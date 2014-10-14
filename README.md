Virtual-Machine
===============

源码结构  
1、代码分为两层，算法层和应用层；   
2、算法层使用桥接和策略混合模式对源码进行组织；    
3、通过继承Accelerator类定义新的模拟加速机制，通过继承Loader类实现新的可执行文件加载器；   
4、通过在应用层选择指定的加载、加速机制，完成虚拟机的创建；    
5、源码使用autotools管理工具对构建进行管理；  
6、测试用例在test文件夹下;    
     
构建方法    
./configure --prefix=... ; make && make install

运行测试用例   
Virtual-Machine image.bin

