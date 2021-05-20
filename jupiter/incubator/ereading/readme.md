# EReading(欢读亭)
## 目标(Target)
> what we want do is optimize reading experience.
- optimize the display of search result of books
- help user to screen better quality results

## 功能(Function)
- Search
    + 查询功能：引擎搜索-->展示-->重排章节列表-->重排章节内容
- Rank

- TODO：
    + 主功能逻辑：
        - 选择排行榜的书：从库中查询出章节目录，并展示(可以强制刷新，重新从主检索网站解析)
        - 检索书籍：走检索流程，如果选择的检索条目已经入库，则从库中读取，否则走完整的检索流程，然后将书名加入待入库列表
    - x23us.com 搜索到之后，不会跳转到章节目录页面，需要再次跳转
    + 完善内容页面：字体、样式 ---> 1 (0.5day)
    + 计划任务: 每三天一次，爬取qidian排行榜信息 ---> 2 (0.5day)
    + 计划任务：每天一次，对每一本入库的书，根据最新章节以及最后更新时间，来确定这本书的主检索网站 ---> 3 (0.5day)
    + 计划任务：每天五次，8、12、17、21、00：根据书籍的主检索网站，更新章节信息 ---> 4 (1day)
    + 待入库列表：检索qidian的书籍信息并入库 ---> 5 (1day)
    + 完成登录注册
    + 完成书架功能
    + 用户阅读书籍记录
    + 完成书籍推荐功能

## reference
- https://github.com/howie6879/owllook
- https://github.com/tkliuxing/bookspider : bookspider 代理池 

- sweetalert
    + guides: https://sweetalert.js.org/guides/#getting-started
    + js: <script src="https://unpkg.com/sweetalert/dist/sweetalert.min.js"></script>
## Tools
- 生成项目依赖：pip3 install pipreqs,  pipreqs  ./ --force
