/*
连接kmboxNet盒子输入参数分别是盒子
ip  ：盒子的IP地址 （显示屏上会有显示）
port: 通信端口号   （显示屏上会有显示）
mac : 盒子的mac地址（显示屏幕上有显示）
返回值：连接成功返回0 ，其他值参见错误代码
*/
int kmNet_init(char* ip, char* port, char* mac);//ok
int kmNet_mouse_move(short x, short y);			//ok
int kmNet_mouse_left(int isdown);				//ok
int kmNet_mouse_right(int isdown);				//ok
int kmNet_mouse_middle(int isdown);				//ok
int kmNet_mouse_wheel(int wheel);				//ok
int kmNet_mouse_all(int button, int x, int y, int wheel);//ok
int kmNet_mouse_move_auto(int x, int y, int time_ms);	//ok
int kmNet_mouse_move_beizer(int x, int y, int ms, int x1, int y1, int x2, int y2);//二阶曲线

//键盘函数
int kmNet_keydown(int vkey);// ok
int kmNet_keyup(int vkey);  // ok

//监控系列
int kmNet_monitor(int enable);			//开启关闭物理键鼠监控
int kmNet_monitor_mouse_left();			//查询物理鼠标左键状态
int kmNet_monitor_mouse_middle();		//查询鼠标中键状态
int kmNet_monitor_mouse_right();		//查询鼠标右键状态
int kmNet_monitor_mouse_side1();		//查询鼠标侧键1状态
int kmNet_monitor_mouse_side2();		//查询鼠标侧键2状态 
int kmNet_monitor_keyboard(short  vk_key);//查询键盘指定按键状态
//物理键鼠屏蔽系列
int kmNet_mask_mouse_left(int enable);	//屏蔽鼠标左键 
int kmNet_mask_mouse_right(int enable);	//屏蔽鼠标右键 
int kmNet_mask_mouse_middle(int enable);//屏蔽鼠标中键 
int kmNet_mask_mouse_side1(int enable);	//屏蔽鼠标侧键键1 
int kmNet_mask_mouse_side2(int enable);	//屏蔽鼠标侧键键2
int kmNet_mask_mouse_x(int enable);		//屏蔽鼠标X轴坐标
int kmNet_mask_mouse_y(int enable);		//屏蔽鼠标y轴坐标
int kmNet_mask_mouse_wheel(int enable);	//屏蔽鼠标滚轮
int kmNet_mask_keyboard(short vkey);	//屏蔽键盘指定按键
int kmNet_unmask_keyboard(short vkey);	//解除键盘指定按键
int kmNet_unmask_all();					//解除屏蔽所有已经设置的物理屏蔽


//配置类函数
int kmNet_reboot(void);									  //重启盒子
int kmNet_setconfig(char* ip, unsigned short port);		  //配置盒子IP地址
int kmNet_setvidpid(unsigned short vid, unsigned short pid);//设置盒子VIDPID 设置后需要重启盒子。下次上电才能生效
int kmNet_debug(short port, char enable);				  //调试使能
int kmNet_lcd_color(unsigned short rgb565);				  //将整个LCD屏幕用指定颜色填充。 清屏可以用黑色
int kmNet_lcd_picture_bottom(unsigned char* buff_128_80); //下半部分显示128x80图片
int kmNet_lcd_picture(unsigned char* buff_128_160);		  //整屏显示128x160图片

