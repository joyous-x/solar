const  searchGarbage = require('../../../utils/garbage-search.js');
var util = require('../../../utils/util.js')


Page(
    {
    data: {
      datas:[],
      searchText:"",
        noResult:"没有查到结果，换个词试试奥~",
      logo:"",
  },

 
  onLoad: function (options) {
      if(options.searchText){
          this.setData({
              searchText:options.searchText
          });
        this.onGetData(options.searchText);
      }
  },
  searchIcon:function(e){
      console.log('search:'+e);
      if(!e.detail.value){
        return ;
      }
      this.setData({
          searchText:e.detail.value
      });
    console.log("====="+e.detail.value)
    this.onGetData(this.data.searchText)
  },
  onGetData:function(text){
    var that=this
    var searchResult = new Array();

     searchGarbage.search(text, function success(res){
         searchResult = res;
         that.setData({
             datas:searchResult
         })
     });

    console.log('searchText:' + that.data.searchText + ' , result:' + JSON.stringify(this.data.datas));
  },
  onItemClick:function(event){
    var index =event.currentTarget.dataset.index
    var logoImg=""
    console.log(index)
    switch (parseInt(index)) {
      case 1:
        logoImg = "../images/RecycleableWaste.jpg"
        break;
      case 2:
        logoImg = "../images/HazardouAwaste.jpg"
        break;
      case 3:
        logoImg = "../images/HouseholdfoodWaste.jpg"
        break;
      case 4:
        logoImg = "../images/ResidualWaste.png"
        break;
    }
    console.log(logoImg)
    this.setData({
      logo:logoImg,
      isShow:!this.data.isShow
    })
  }
  
})