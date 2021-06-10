# Antenna
## TODO: 
### 并发以及控制
- Thread
```
    threads = []
    for url in fetch_list:
        t = Thread(target=do_action, args=[do_action_args])
        t.start()
        threads.append(t)

    for t in threads:
        t.join()
```
- async、await
```
generator coroutines
    requests库改成支持asyncio的aiohttp库, 使用3.5的async/await语法
（3.5之前用@asyncio.coroutine和yield from代替）写出协程版本

    loop = asyncio.get_event_loop()
    res = loop.run_until_complete(asyncio.wait([do_action("a"), do_action("b")]))
    loop.close()

async def 的語法 引入了原生協程(native coroutine)或異步生成器(asynchronous generator)。
關鍵字 await 將功能控制權回傳給事件循環(event loop)。(它將暫停周圍 coroutine 的執行, 直到被 await 的函数执行完成) 
使用 async def 宣告的 function 是 coroutine。它可以使用 await，return 或yield，但是所有這些都是可選的。
使用 await and/or return將創建協程函數(coroutine functions)。要使用協程函數(coroutine functions)，必須使用 await 來獲得結果。

協程(coroutine)是增強型生成器(generator)。

除了 async / await 之外，Python 還提供了 async for 讓我們可以進行 asynchronous 的迭代(asynchronous iterator)。
與普通 的async / await 一起，Python 還使 async 用於在異步迭代器上進行迭代。異步迭代器的目的是使它能夠在迭代時在每個階段調用異步代碼。

The Event Loop and asyncio.run()
您可以將事件循環視為類似於無限循環，該循環監視協程，持續尋找當進入到空閒狀態時可以接著執行的事情。這個循環同時也有能力喚醒一個空閒的協程，當這個協程從等待的狀態變成可用的狀態。
到目前為止，整個事件循環的管理是由一個函數來處理：
$ asyncio.run(main())
Python 3.7中引入的 asyncio.run（）負責獲取事件循環，運行任務直到將其標記為完成，然後關閉事件循環。
使用get_event_loop（），可以更輕鬆地管理異步事件循環。典型的模式如下所示：
loop = asyncio.get_event_loop()
try:
    loop.run_until_complete(main())
finally:
    loop.close()
在較早的示例中，您可能會看到 loop.get_event_loop（）隨處可見，但是除非您特別需要對事件循環管理的控制做微調，否則 asyncio.run（）對於大多數程序而言就足夠了。

請記住，asyncio.sleep（）用於模仿其他一些更複雜的協程(coroutine)，如果這是常規的阻止函數(blocking function)，則會消耗時間並阻止所有其他執行
異步IO與多處理(multiprocessing)之間的鬥爭根本不是一場爭鬥。實際上，它們可以一起使用。如果您有多個 CPU-bound 的任務（一個很好的例子是在scikit-learn或keras之類的庫中進行網格搜索），那麼多處理應(multiprocessing)該是一個明顯的選擇。
如果所有函數都使用阻塞形式的，那使用異步IO是一個壞主意。（實際上可能會使您的代碼變慢。）但是，如前所述，異步IO和多處理(multiprocessing)在某些地方可以和諧共處。
異步IO與線程(thread)之間的競爭更為直接。我在引言中提到"threading is hard."。全文是，即使在線程似乎易於實現的情況下，由於競爭條件(race condition)和內存使用等原因，它仍然可能導致無法被追蹤的 bugs。
由於線程是具有有限可用性的系統資源，因此線程的擴展也往往比異步IO優雅。創建數千個線程將在許多計算機上失敗，因此我不建議您首先嘗試。創建數千個異步IO任務是完全可行的。

!!! reading --- https://zhuanlan.zhihu.com/p/27258289
          https://blog.taiker.space/python-async-io-in-python-a-complete-walkthrough/
          https://realpython.com/async-io-python/#setting-up-your-environment
```
- Process
```
    from concurrent.futures import ProcessPoolExecutor

    with ProcessPoolExecutor(max_workers=4) as executor:
        for page in executor.map(do_action, do_action_args_list):
            html = etree.HTML(page)
            for element_movie in html.xpath(xpath_movie):
                result.append(element_movie)

(注：ThreadPoolExecutor和ProcessPoolExecutor是Python3.2之后引入的分别对线程池和进程池的一个封装，如果使用Python2.x，需要安装futures这个库才能使用它们。)
```

yield能把一个函数变成一个generator

asyncio.Future
from collections import deque
from selectors import DefaultSelector, EVENT_WRITE
