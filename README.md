# FreeRTOS 菜单系统

基于 STM32F103 + FreeRTOS 的嵌入式菜单系统，采用**多叉树链表**管理页面，通过**消息队列**实现任务间通信。

---

## 链表节点结构

菜单系统的核心是一个多叉树结构的链表节点，定义在 `User/APP/lnc/node.h` 中：

```c
typedef struct node{
    void (*display)(void);      // 显示函数指针
    void (*action)(void);       // 确认/执行函数指针
    void (*move)(void);         // 移动函数指针（切换调用）
    struct node *next;          // 下一个兄弟节点
    struct node *prev;          // 上一个兄弟节点
    struct node *father;        // 父节点
    struct node *child;         // 子节点
} Node;
```

### 节点关系

```
        root (main menu)
          |
       [child]
          |
      root_back ──> setting ──> setting_back ──> light
                    |                               |
                 [child]                        [child]
                    |                               |
                (子菜单)                         (子菜单)
```

- **next / prev** — 水平方向，连接同级兄弟节点（如 `root_back`、`setting`、`light`）
- **father / child** — 垂直方向，连接父子节点（如 `root` → `root_back`）

### 函数指针分工

| 指针 | 作用 |
|------|------|
| `display` | 进入该节点时调用，负责 OLED 绘制当前页面 |
| `action` | 按键"确认"时调用，执行该页面绑定的功能 |
| `move` | 按键"移动"时调用，切换 `current_node` 到相邻节点 |

### 内存分配

使用静态数组预分配，避免堆碎片：

```c
Node menu[MAX_MENU_NODES];   // 预分配 20 个节点
```

`malloc_static()` 依次返回数组中的空位，无需 `malloc/free`，适合嵌入式场景。

---

## 任务分配

在 `MX_FREERTOS_Init()` 中创建三个任务 + 1 个定时器：

```
┌────────────────────┬────────┬──────────┬───────────────────────────┐
│ 任务名             │ 优先级 │ 栈大小   │ 职责                      │
├────────────────────┼────────┼──────────┼───────────────────────────┤
│ KeyScan_Task       │ 27     │ 64 words │ 扫描按键，发送按键事件      │
│ Menu_Task          │ 26     │ 256 words│ 处理按键事件，驱动菜单导航  │
│ Display_Task       │ 25     │ 128 words│ 根据消息刷新 OLED 显示      │
│ refresh_timer      │ —      │ —        │ 20ms 周期定时器，防触刷新   │
└────────────────────┴────────┴──────────┴───────────────────────────┘
```

> FreeRTOS 中优先级数值越大，优先级越高。`KeyScan_Task` 优先级最高，确保按键及时响应。

### 任务职责详解

#### KeyScan_Task
```c
void KeyScan_Task(void *arg)
```
- 循环调用 `key_scan(k)` 检测按键状态
- 将非 `KEY_EVENT_NONE` 的事件发送到 `key_event_queue`
- 周期 `vTaskDelay(10ms)` 消抖

#### Menu_Task
```c
void Menu_Task(void *arg)
```
- 阻塞接收 `key_event_queue`
- 根据事件类型调用 `current_node->move()` 或 `current_node->action()`
- 动作完成后发送 `DISPLAY_MSG_REDRAW` 到 `display_queue`

#### Display_Task
```c
void Display_Task(void *arg)
```
- 阻塞接收 `display_queue`
- 调用 `current_node->display()` 绘制当前页面
- `OLED_Update()` 刷新屏幕

---

## 任务间通信

```
KeyScan_Task          Menu_Task             Display_Task
     │                    │                      │
     │  key_event_queue   │                      │
     │──────────────────>│                      │
     │                    │  display_queue       │
     │                    │─────────────────────>│
     │                    │                      │
     │              (refresh_timer 20ms)         │
     │                    │   若仍在 head 重绘      │
     │                    │─────────────────────>│
```

| 资源 | 类型 | 大小 | 用途 |
|------|------|------|------|
| `key_event_queue` | Queue | 10 × `KeyEvent_t` | 传递按键事件 |
| `display_queue` | Queue(overwrite) | 1 × `DisplayMsg_t` | 触发屏幕刷新 |
| `refresh_timer` | Timer | 20ms 周期 | 定时刷新页面（如闪烁光标） |

---

## 文件结构

```
Core/Src/freertos.c         任务创建、队列初始化、业务逻辑
User/APP/lnc/node.h         Node 结构体定义 + MAX_MENU_NODES
User/APP/Src/node.c         节点创建/添加/菜单构建
User/APP/Src/UI.c           各页面的 display 函数实现
User/APP/Src/UI.h           DisplayMsg_t 枚举
User/APP/Src/action.c       各页面的 action 函数实现
User/APP/Src/key_scan.c     按键驱动
User/BSP/Src/oled.c         OLED 驱动
```