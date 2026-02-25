# op-log.md
> 操作(operator)记录文档日志(log)

```txt
下面内容的总结：
	从翻译来的单词表,层层过滤,做成base,取2字词, 3字词
	再补充8105和idiom, 加入上版本的做好的1码和2码(大量修改),
	经过过滤, 调序, 整合成码表
```

# 单字表(share/encoder-data-single/norm8105)
> norm8105的1、2、3级共8105个通用规范字(作为词表的所含字的全集，编码词表的基础)
> 预期：网络收集的并整理的wb86-u8set为基础能完全覆盖norm8105

```shell
执行: ./bin/encoder wb86-u8set norm8105empty > norm8105

error: 出现缺失, 修补wb86-u8set

(收集补充更多编码)
多次执行补充: ./bin/unite wb86-u8set-last addition > wb86-u8set

执行: ./bin/encoder wb86-u8set norm8105temp > norm8105
无缺失, works well
```


# 词表基础(share/base/from-words)
> 理由：收集含来自世界上各学科的`翻译者`的词汇，`人名过滤`得到很好的支持，`实用`且`广泛`，etc.
> 从from-words的中英词译作为基础,这意味着最终的词表必是(此表+norm8105单字集)的子集

> wordlist: git@github.com:condexpr01/q-dictionary
> 正则匹配含人名tag的行删除
> 正则替换所有ascii 0x00-0x7f为\n: `perl -pe 's/[\x00-\x7f]/\n/g' < wordlist > wordlist-base`
> 正则删除\n开头的行: `perl -pe 's/^\n//g' < wordlist-base > wordlist`
> 对中文符号也做一遍替换为\n再去除
> 正则格式化
> 现在以编码器提取不能编码的(8105为基础,sort err.txt > uerr.txt)

> uerr.txt再过滤后
```text
¨°±·×–—″↑∑∧∨∶─│┊┌┍┎┏┐┑┒┓└┕┖┗┘┙┚┛├┟┡┢┣┤┥┦┧┨┩┪┫┭┮┯┰┱
┲┳┴┵┶┷┸┹┺┻┼┽┾┿╁╂╃╄╆╇╈╉╱■▽○◎☉〈〉《》『〔〕〖〗︒︽﹙！％＆＋－．／＜＝？［
］～＄￡０１①２②③４５７áàǎāɑＣéèêěēＦɡⅠíìǐīⅡⅢⅣḿńóòǒōùǔüǜǖūⅤⅧｘＸⅪ
αβγΓδΔεζηθικλΛμΞπΠρσΣτφΦχψωΩаАбБвВгГдДеЕёЁжЖзЗИйЙкК
лЛмМНоОпПрРсСуфФхХЦчЧшШщЩъЪыЫЬэЭюЮЯ
```

> 正则将wordlist去格式化,再做一遍替换uerr.txt中字符为\n再去除
> 以sort -u 过滤wordlist

> encoder编码器逻辑带来的问题:编码n>5长的串时，[4,n)的并不会检查,导致uerr不全
> 现在过滤大于5长的(5长以上的翻译过于口语化，迟早得搞去的)

> 意外的干净去除5长后，翻译作为源比预期还好点
> uerr.txt fixed content
```text
┬』６ňúǘТ
```
> 再做一遍替换uerr.txt中字符为\n再去除

> 不假设非8105字集中的字会作为seperator，直接格式化，再encoder_with-filter过滤
> `./bin/encoder-with-filter share/encoder-data-single/norm8105 share/base/wordlists > share/base/from-words`


# wordlist过滤
> 与pinyin取交集，(提高可拼读性) `./bin/intersect from-words pinyin > d-alpha`
> 与频表freq取交集(提高常用性),并获取频数 `./bin/intersect d-alpha freq > d-beta`

> error: pinyin和freq还是感觉太小,过渡过滤,我的from-words过滤后没有剩多少(非预期行为)
> 考虑补充pinyin和freq(爬取数据,分词统计)

```
> freq: 正则化数据，./bin/unite并入频数，现在freq体积是line=4511505

./bin/get-freq-with-filter share/freq/freq pinyin > pin
sort -u pin > pin-alpha
./bin/freq-sort pin > pinyin
> pinyin: 正则化数据,./bin/pinyin-normalize正常化数据，./bin/unite并入数据，现在pinyin体积是line=938365
```

> 重新：
> 与pinyin取交集，(提高可拼读性) `./bin/intersect from-words pinyin > d-alpha`
> 与频表freq取交集(提高常用性),并获取频数 `./bin/intersect d-alpha freq > d-beta`
> waring: from-words虽然是来自翻译，但出现问题，高频的漏了些，难以接受

> fix: 以./bin/encoder-with-filter使freq的字符在8105字集中
> `./bin/encoder-with-filter share/encoder-data-single/norm8105 share/freq/freq > freq8105`
> fix: 取阈值(前1w行,最低频数418063)以上的子集unite到from-words
> `./bin/freq-sort freq8105 > freq8105sorted`然后再去除5长及以上的
> thinking: 不取pinyin的高阈值，取freq的，之后再把并入后的from-words和pinyin相交时，可拼读性就有了

> 现在经过encoder,get-freq,freq-sort得到out-norm-freq-sort表，
> 去除表中的低频噪音
> 去除4长(太口语化了，补idiom进去，nice)
> 得到质量不错的`2长词语`和`3长词语`

> 得宜于从翻译来的源，2长也不错
> 得宜于从翻译来的源，像`每一个`,`为什么`这样的实用3长很好,噪声还少


# 反查制作

```shell
./bin/freq-sort share/pinyin/pinyin > py
取py的前(20w-1000)行
取出被去除部分的单字补充到py

./bin/make-seq py > pyq
匹配检查调整`词`过密集情况

./bin/freq-sort pyq > pyqs
正则化pyqs为rime表格式
```

# 码表整合

* 数据描述

```txt
现在有：

# 来自单词翻译的高质量表,长2
# 来自单词翻译的高质量表,长3
# 高频，长2（目的：现代化
# 高频，长3（目的：现代化
# 高频，长4（目的：现代化
# 成语，长4，looks good to me
# 保留上版词典的1码
# 保留上版词典的2码
```

> dict: (频数只能作为参考，不应该做为排序)

> 1码:
```txt
1号: 1级简码带调整
2号: 键名字带调整
```

> 2码:
```txt
1号`直观2码单字`

2号和3号调整高频和容易记住的叠叠字,
特别的,在2码2号非词的几个:

> fc,1=去,61017811
> fc,2=云,3116832

> fq,1=元,55885060
> fq,2=无,19124730

> ft,1=才,22655080
> ft,2=都,106861515

> yy,1=方,10421417
> yy,2=文,8657301
```

> 3码: 
```txt
1号：
> `全码为3的单字`占位，如果有空出的1号位用高频4码字补


2号及以上：候选

3长词补位(技巧:把3长的序号放大，再用codec-seq-sort)
```

> 4码: 
```txt
1号及以上：全码(按make-seq生成序号并调整)
```



# 码表调整
> 预调整了部分, 对表做了些改序，去除数字，英文人名等一系列的

> 后期准备在使用中调整






