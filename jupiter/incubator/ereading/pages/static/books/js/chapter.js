//--------------------------------------------------------------------------------------------

$(document).ready(function () {
    var page_btn_pre = $("div.pre_next > a:nth-child(1)");
    var page_btn_next = $("div.pre_next > a:nth-child(2)");
    var page_title = $("title");
    var page_chapter_name = $("#cptitle");
    var page_chapter_content = $($(".show-content").children("*").get(0)); //正文
    var page_bookmark = $("#bookmark"); //需要修改样式

    ajax_content_init();

    function isSessStorageSupport() {
        return (typeof window.sessionStorage != "undefined");
    }
    function store(urlpath, obj) {
        window.sessionStorage.setItem(urlpath, JSON.stringify(obj));
    }
    function ajax_content_init() {
        if (!isSessStorageSupport()) {
            return ;
        }
        location = window.location;
        if (location.search.indexOf("src=bookmarks") > 0) {
            return ;// 来自书签页面的跳转不进行缓存
        }
        ajax_task();
        page_bookmark.bind("click", function () {
            window.sessionStorage.clear();
            ajax_task();
        });
    }
    function ajax_task() {
        page_btn_pre.unbind("click");
        page_btn_pre.click(function () {
            event.preventDefault();
            url_prepage = page_btn_pre.attr("href")
            if (window.sessionStorage.getItem(url_prepage) === null) {
                window.location.href = url_prepage
                get_chapter(pre_url);
            } else {
                try {
                    load_cached_page(url_prepage);
                } catch (err) {
                    window.location.href = url_prepage;
                    get_chapter(pre_url);
                }
            }
        });
        page_btn_next.unbind("click");
        page_btn_next.click(function () {
            event.preventDefault();
            url_nextpage = page_btn_next.attr("href")
            if (window.sessionStorage.getItem(url_nextpage) === null) {
                window.location.href = url_nextpage;
                get_chapter(url_nextpage);
            } else {
                try {
                    load_cached_page(url_nextpage);
                } catch (err) {
                    window.location.href = url_nextpage;
                    get_chapter(url_nextpage);
                }
            }
        });
    }
    function load_cached_page(index) {
        var data = JSON.parse(window.sessionStorage.getItem(index));

        page_bookmark.removeClass("bookmark");
        page_bookmark.removeClass("bookmarkAct");
        if (data.bookmark == 0) {
            page_bookmark.addClass("bookmark");
        } else {
            page_bookmark.addClass("bookmarkAct");
        }

        page_title.html("{{appname}} - " + data.name);
        page_chapter_name.text(data.name);

        page_btn_pre.attr("href", "/chapter?url=" + data.pre_chapter_url + "&cpurl=" + data.chapter_url + "&bname=" + data.novels_name);
        page_btn_next.attr("href", "/chapter?url=" + data.next_chapter_url + "&cpurl=" + data.chapter_url + "&bname=" + data.novels_name);

        var th_url = window.location.href + "";
        var pos = th_url.indexOf("/chapter?");
        var td_url = "/chapter?url=" + data.url + "&name=" + data.name + "&cpurl=" + data.chapter_url + "&bname=" + data.novels_name;
        th_url += td_url;
        // 更新地址栏URL 
        window.history.replaceState({}, data.name + " - {{appname}}", td_url);

        page_chapter_content.html(stripscript($(data.content).html()));
        $("body > div.container.all-content > div.move > div.move_up").click();//回到顶部
        ajax_task();
    }

    function stripscript(s) { //用于过滤script标签
        return s.replace(/<script>.*?<\/script>/ig, '').replace(/<.*?div.*?>/, '');
    }

    function get_chapter(urlpath, bname) {
        $.ajax({
            type: "POST",
            url: "/book/chapter",
            data: {src: "ajax", url:urlpath, bname=bname},
            dataType: 'json',
            contentType: "application/json",
            //beforeSend:function(){$("#msg").html("logining");},
            success: function (resp) {
                if (typeof resp.data == "undefined") {
                    log("ajax query data.name==undefined : path =" + urlpath)
                    return
                }
                data = resp.data
                var obj = {
                    cpurl: urlpath,
                    pre_chapter_url: transform(data.next_cpurl)[0],
                    next_chapter_url: transform(data.next_cpurl)[1],
                    cptitle: data.cptitle,
                    bname: data.bname,
                    content: data.cpcontent,
                    chapter_url: data.chapter_url
                };
                store(urlpath, obj);
            },
            complete: function () {},
            error: function () {}
        });
    }

    function transform(obj) {
        var arr = [];
        for (var item in obj) {
            arr.push(obj[item]);
        }
        return arr;
    }

    function log(s) {
        console.log(s);
    }

    $(document).keydown(function (event) {
        var e = event || window.event;
        var k = e.keyCode || e.which;
        switch (k) {
            case 39:// right
                page_btn_next.click();
                break;
            case 37:// left
                page_btn_pre.click();
                break;
            case 38:// up
                break;
            case 40:// down
                break;
        }
        return true;
    });

});
