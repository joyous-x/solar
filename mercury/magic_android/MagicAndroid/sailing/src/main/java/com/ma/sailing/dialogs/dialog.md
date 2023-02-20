

# 自定义 Dialog


```
    // 实例化 Dialog 对象，并应用自定义的 NormalDialogStyle 去掉系统样式
    Dialog dialog = new Dialog(this,R.style.NormalDialogStyle);

    // 给 dialog 设置一个视图
    View view = View.inflate(this, R.layout.div_dialog, null);
    dialog.setContentView(view);

    // 在点击窗口外的地方可以退出
    dialog.setCanceledOnTouchOutside(true);

    // 实例化窗口布局对象
    Window dialogWindow = dialog.getWindow();
    WindowManager.LayoutParams lp = dialogWindow.getAttributes();
    lp.width = WindowManager.LayoutParams.MATCH_PARENT; // 宽度铺满
    lp.height = WindowManager.LayoutParams.WRAP_CONTENT; // 高度自适应
    lp.gravity = Gravity.BOTTOM; // 窗口停靠在底部居中
    dialogWindow.setAttributes(lp);

    dialog.show();
```