# cpp-serialization

#### 介绍
C++对象序列化、反序列化，
使用C11与rapidJson，自动完成序列化与反序列化，
包含头文件即可直接使用，无其他依赖。

#### 软件架构
借助C++11新特性 + 宏 + rapidjson


#### 安装教程

1.  复制source中的文件到你的工程
2.  include "serialize.hpp"
3.  即可使用

#### 使用说明

1.  简单对象使用如下：

```
    PropertyObject<int> my("Speed",10);
    my.Set(333);
    std::string str = my.ToSerializeString(); //序列化

    PropertyObject<int> my2("Speed", 0);
    my2.FromSerializeString(str );
    int nSpeed = my2.Get();
```


2.  对象组合用法，详见example文件夹
3.  宏的使用 

#### 参与贡献

1.  Fork 本仓库
2.  新建 Feat_xxx 分支
3.  提交代码
4.  新建 Pull Request


