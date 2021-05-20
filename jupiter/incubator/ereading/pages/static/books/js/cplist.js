//---------------------

$(document).ready(function () {
    var cphost = $("#cphost").val();
    var cplisturl = $("#url").val();
    var cptype = $("#cptype").val();
    var bname = $("#bname").val();
    var cpscheme = $("#cpscheme").val();
    $(".container a").each(function () {
        var cptitle = $(this).text();
        var cpurl = $(this).attr('href');
        if (typeof(cpurl) != "undefined") {
            if (cpurl.indexOf("http") < 0) {
                if (cptype == 'join_host') {
                    showUrlPrefix = "chapter?url=" + cpscheme + "://" + cphost + cpurl;
                } else if (cptype == 'join_cplist') {
                    showUrlPrefix = "chapter?url=" + cplisturl + cpurl;
                } else {
                    showUrlPrefix = "error?url=" + cpurl;
                }
            } else {
                showUrlPrefix = "chapter?url=" + cpurl;
            }
            showUrl = showUrlPrefix + "&title=" + cptitle + "&cplist=" + cplisturl + "&bname=" + bname;
            $(this).attr('href', showUrl);
            $(this).attr('target', '_blank');
        }
        console.log("cplist ready: bname=%s cpurl=%s cptype=%s showUrlPrefix=%s", bname, cpurl, cptype, showUrlPrefix)
    });
});