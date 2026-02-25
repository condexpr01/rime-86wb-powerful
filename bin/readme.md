
# 工具编译
```shell
cd src
make
```

# 标准格式
> codec,seq=word,freq
```txt
以`编码,序号=字词,频数`形成的多行utf-8文本

如：
wqvb,1=你好,1661360
wqvb,2=您好,378383
wv,1=分,23956465
wv,2=你好,1661360
wv,3=做好,612346

使用时也可以只含需要的工具部分,如encoder可用的被编码表:
,=你好,
,=您好,
,=分,
,=你好,
,=做好,
```

# 工具预览

```shell
  bin
     gen-readme            # 给标准格式码表生成readme

     fmt-to-rime           # 标准格式到rime
     fmt-to-sougou         # 标准格式到sougou
     rime-to-fmt           # rime original到标准格式

     breakup-codec         # 标准格式按编码长度拆分
     breakup-word          # 标准格式按字词长度拆分
     breakups-merge        # 合并拆分

     codec-seq-sort        # 标准格式,按编码排序,同编码的按序号排,同时修补序号
     codec-set-difference  # 标准格式,按编码计算表1和表2的差集(表1中存在而表2中不存在)
     codec-set-intersect   # 标准格式,按编码计算表1和表2的交集
     codec-set-unite       # 标准格式,按编码计算表1和表2的并集

     word-set-difference   # 标准格式,按字词计算表1和表2的差集(表1中存在而表2中不存在)
     word-set-intersect    # 标准格式,按字词计算表1和表2的交集
     word-set-unite        # 标准格式,按字词计算表1和表2的并集

     encoder               # 标准格式,编码器,对字词生成编码
     encoder-with-filter   # 标准格式,编码器,过滤不能编码的字词

     freq-sort             # 标准格式,按频数排序

     get-freq              # 标准格式,为码表从频表中获取频数
     get-freq-with-filter  # 标准格式,为码表从频表中获取频数,过滤得不到频数的

     make-seq              # 标准格式,以频数为依据生成序号
     make-seq-wbfilter     # 标准格式,以频数为依据生成序号,同时过滤在3号位以后的词

     ollama-filter         # 标准格式,尝试以ollama ai模型生成过滤信息

     pinyin-normalize      # 标准格式,多编码的拼音变单编码正常化
```



# 工具详细

* 简易格式变换

```shell
若文件a内容为:
wqvb,2=您好,378383
wqvb,1=你好,1661360
wv,3=做好,612346
wv,1=分,23956465
wv,2=你好,1661360

运行: fmt-to-rime a
结果:
wqvb	你好
wqvb	您好
wv	分
wv	你好
wv	做好

运行: fmt-to-sougou a
结果:
wqvb,1=你好
wqvb,2=您好
wv,1=分
wv,2=你好
wv,3=做好

###################

文件b内容为:
wqvb	你好
wqvb	您好

运行: rime-to-fmt b
结果:
wqvb,=你好,
wqvb,=您好,
```


* 拆分

```shell
文件a内容为:
d,=,
dd,=,
ddd,=,
dddd,=,

运行: breakup-codec a
结果: breakup1.txt ~ breakup4.txt内容对应: d dd ddd dddd

文件b内容为:
,=,字
,=,字字
,=,字字字
,=,字字字字
,=,字字字字字

运行: breakup-word b
结果: breakup1.txt ~ breakupn.txt内容对应: 字 字字 字字字 字字字字

拆分目录下运行: breakup-word
结果：合并后的数据
```



* 排序且修补
```
文件a内容为:
c,1=又,
wv,10=分,
b,=子,
wv,20=你好,
a,1=工,
wv,30=做好,

运行: codec-seq-sort a
结果: 
a,1=工,
b,1=子,
c,1=又,
wv,1=分,
wv,2=你好,
wv,3=做好,

```

* 集合运算
```
文件a内容为:
a,1=工,
b,1=了,

文件b内容为:
b,1=了,
c,1=以,

运行: codec-set-difference a b
结果(a中存在b中不存在):
a,1=工,

运行: codec-set-intersect a b
结果(a中存在b中存在):
b,1=了,

运行: codec-set-unite a b
结果(a和b的元素):
a,1=工,
b,1=了,
c,1=以,


word-set-difference,word-set-intersect,word-set-unite逻辑和上面一样
但是参考的是字词字段word,同时如果存在频数，会相加



#####################
从github仓库上可以找到人名，地名等仓库，取数据转换为标准格式后，
用word-set-difference就可以去人名，地名等

统计生成的数据频表,可以用word-set-unite取并集,如
a,1=工,1111
a,1=工,2222
会并为:
a,1=工,3333

也可以用word-set-interset只统计表中已有的数据
```

* 编码器
```shell
若文件a内容为:
,=你,
,=你好,
,=0139,

# 用8105表编码a
运行: encoder share/encoder-data-single/norm8105 a
结果:
wqyi,=你,
wqvb,=你好,
,=0139,

运行: encoder-with-filter share/encoder-data-single/norm8105 a
结果:
wqyi,=你,
wqvb,=你好,
```

* 频数操作

```shell
若文件a内容为:
,=区区,203656
,=式,3816084
,=某某,391924

运行: freq-sort a
结果: 
,=式,3816084
,=某某,391924
,=区区,203656

####################

若文件b内容为:
,=区区,
,=式,
,=某某,
,=&&&&&&,

# 从频表获取频数
运行: greq-freq share/freq/freq b
结果: 
,=区区,203656
,=式,3816084
,=某某,391924
,=&&&&&&,

运行: get-freq-with-filter share/freq/freq b
结果: 
,=区区,203656
,=式,3816084
,=某某,391924
```


* 从频数生成序号

```shell

若文件a内容为:
aa,=,203656
aa,=,3816084
aa,=,391924


运行: make-seq a
结果: 
aa,1=,3816084
aa,2=,391924
aa,3=,203656

```


* 多音字正常化

```
pinyin-normalize
# 标准格式,多编码的拼音变单编码正常化

若文件a内容为:
xing/hang,=行,

运行: pinyin-normalize a
结果: 
xing,=行,
hang,=行,
```

