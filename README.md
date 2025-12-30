# Windows 通用控件 UI开发模板



这个仓库包括一个windows 通用控件UI开发模板。该UI开发模板基于Windows 本地API，具有零依赖、轻而美、通用等特性。该开发模板来源于WintoolsSuite里的UI工具。



该模板目前具有如下控件：

1. 菜单栏（Menu Bar）
2. 工具栏（Tool Bar）
3. Rebar（放置Toolbar、QuickSearch控件的容器）
4. 状态栏（Status Bar）
5. 列表视图（表）
6. 对话框 （Dialog Bar）



本地Window API：

1. CreateWindowEx
2. CreateDialogParam
3. DialogBoxParam
4. SendMessage
5. MoveWindow
6. SetWindowPos
7. GetClientRect/GetWindowRect/ScreenToClient
8. WindowProc（窗口过程）
9. DialogProc（对话框过程）
   
窗口消息：

WM\_SIZE -- 基于MFC开发的UI，当UI窗口尺寸发生变化时，MFC会自动基于UI上控件的初始布局来缩放控件。而当基于本地Windows API开发的UI，需要处理WM\_SIZE消息，
手动重新调整那些随UI大小变化的控件。当调整子控件的大小时，使用GetClientRect API来获取父控件的工作区，使用GetWindowRect来获取子控件相对于屏幕的坐标和尺寸，
然后使用ScreenToClient来将子控件的坐标转换为相对于父控件的坐标，以使用MoveWindow/SetWindowPos API来重新设置子控件的位置和大小。
基于区域中的最底层控件来确定该区域的高度，以自动适应不同的系统和分辨率。

WM\_INITMENUPOPUP -- 菜单弹出时更新菜单项的状态，例如启用/禁用、Check/UnCheck等

WM\_TIMER -- 定时器

LVN\_COLUMNCLICK -- 列表表头列单击消息，处理列排序

NM\_RCLICK -- 列表区域右键单击消息，处理右键弹出菜单

NM\_DBLCLK -- 列表行双击消息

LVN\_ITEMCHANGED -- 列表视图条目改变时，更新菜单及Toolbar Button状态

Quick Search -- Hook Edit控件的WndProc，处理WM_KEYDOWN事件，触发定时器来启动搜索

窗口命令：

OnFont -- 为视图选择字体

OnAlwaysOnTop -- 工具窗口置顶

工具栏（Tool Bar）：
1. 增加ToolBar Button状态处理：Check/UnCheck

开发环境：

1. Windows 11
2. Microsoft Visual Studio Community 2022
