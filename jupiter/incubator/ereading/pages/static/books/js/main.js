//---------------------

$(function () { $('[data-toggle="popover"]').popover(); });

$(document).ready(function () {
    $('.move_up').click(function () {
        $('html, body').animate({scrollTop: 0}, 'slow');
        return false;
    });
    $('.move_down').click(function () {
        $('html, body, .content').animate({scrollTop: $(document).height()}, 'slow');
        return false;
    });
    $('#bookrack').click(function () {
        var cpurl = $("#cpurl").val();
        var bname = $("#bname").val();
        if ($(this).hasClass('add-color')) {
            $.ajax({
                type: "post",
                contentType: "application/json",
                url: "/v1/bookrack/del",
                data: {"bname": bname, "cpurl": cpurl},
                dataType: 'json',
                success: function (data) {
                    if (data.status == 0) {
                        $('#bookrack').removeClass('add-color');
                    } else if (data.status == 1) {
                        alert('请登录后管理书架');
                    }
                }
            });
        } else {
            last_read_url = window.location.pathname + window.location.search;
            $.ajax({
                type: "post",
                contentType: "application/json",
                url: "/v1/bookrack/add",
                data: {"bname": bname, "cpurl": cpurl, 'last_url': last_read_url},
                dataType: 'json',
                success: function (data) {
                    if (data.status == 1) {
                        $('#bookrack').addClass('add-color');
                        alert('已成功加入书架~');
                        window.location.reload();
                    } else if (data.status == -1) {
                        alert('请登录后管理书架');
                    }
                }
            });
        }
    });
    $('#bookmark').click(function () {
        var cpurl = $("#cpurl").val();
        var bname = $("#bname").val();
        var cptitle = $("#cptitle").val();
        var cplisturl = $("#cplisturl").val();
        bookmarkurl = "/chapter?cplisturl=" + cplisturl + "&cptitle" + cptitle + "&cpurl=" + cpurl + "&bname=" + bname;
        if ($(this).hasClass('bookmark')) {
            $.ajax({
                type: "post",
                contentType: "application/json",
                url: "/v1/bookmark/add",
                data: {'bookmark_url': bookmarkurl},
                dataType: 'json',
                success: function (data) {
                    if (data.status == 0) {
                        $('#bookmark').removeClass('bookmark');
                        $('#bookmark').addClass('bookmarkAct');
                    } else if (data.status == 1) {
                        alert('请登录后管理书签');
                    }
                }
            });
        } else {
            $.ajax({
                type: "post",
                contentType: "application/json",
                url: "/v1/bookmark/del",
                data: {'bookmarkurl': bookmarkurl},
                dataType: 'json',
                success: function (data) {
                    if (data.status == 0) {
                        $('#bookMark').removeClass('bookMarkAct');
                        $('#bookMark').addClass('bookMark');
                    } else if (data.status == 1) {
                        alert('请登录后管理书签');
                    }
                }
            });
        }
    });
    $("#btnLogin").click(function () {
        var user = $("#user").val();
        var passwd = $("#passwd").val();
        if (user == "" || passwd == "") {
            swal("用户名密码不能为空！");
        } else {
            $.ajax({
                type: "post",
                contentType: "application/json",
                url: "/v1/user/login",
                data: {'uid': user, 'passwd': passwd},
                dataType: 'json',
                success: function (data) {
                    if (data.code == 0) {
                        location.reload();
                    } else {
                        swal("ERROR", "用户名、密码错误!", "error");
                    }
                }
            });
        }
    });
    $("#aLogout").click(function () {
        $.ajax({
            type: "post",
            contentType: "application/json",
            url: "/v1/user/logout",
            dataType: 'json',
            success: function (data) {
                if (data.code == 0) {
                    location.reload();
                }
            }
        });
    })
});

$('.wechat-button').popover({
    trigger: 'hover',
    html: true,
    content: "<img width='120px' height='120px' src='static/novels/img/wxqrcode.jpg'><p style='text-align: center'><span>关注公众号</span></p>"
});
