# cpp-serialization

#### Description
C++ Serialize、DeSerialize，
使用C11与rapidJson，自动完成序列化与反序列化，
包含头文件即可直接使用，无其他依赖。

#### Software Architecture
C11 with rapidjson

#### Installation

1.  copy dir "source" to your project.
2.  include "serialize.hpp".
3.  you can use now.

#### Instructions

1.  simple object use

```
    PropertyObject<int> my("Speed",10);
    my.Set(333);
    std::string str = my.ToSerializeString(); //序列化

    PropertyObject<int> my2("Speed", 0);
    my2.FromSerializeString(str );
    int nSpeed = my2.Get();
```

2.  for multi object use you can find example in dir example

#### Contribution

1.  Fork the repository
2.  Create Feat_xxx branch
3.  Commit your code
4.  Create Pull Request


