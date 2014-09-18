/**
 * DashboardController
 *
 * @description :: Server-side logic for managing users
 * @help        :: See http://links.sailsjs.org/docs/controllers
 */

module.exports = {

  index: function(req, res) {

    var steps = [
      {active: "active", title: "總覽"},
      {active: "", title: "Φ 別"},
      {active: "", title: "月份"},
      {active: "", title: "週"},
      {active: "", title: "日期"},
      {active: "", title: "機器"}
    ];

    res.view("total/overview", {dataURL: "/api/json/total", steps: steps});
  },

  product: function(req, res) {
    var productName = req.param("product");
    var productDataURL = "/api/json/total/" + productName;

    var steps = [
      {active: "active", title: "總覽"},
      {active: "active", title: productName},
      {active: "", title: "月份"},
      {active: "", title: "週"},
      {active: "", title: "日期"},
      {active: "", title: "機器"}
    ];

    res.view("total/overview", {dataURL: productDataURL, steps: steps});
  },

  productMonth: function(req, res) {
    var productName = req.param("product");
    var productMonth = req.param("month");

    var steps = [
      {active: "active", title: "總覽"},
      {active: "active", title: productName},
      {active: "active", title: productMonth},
      {active: "", title: "週"},
      {active: "", title: "日期"},
      {active: "", title: "機器"}
    ];

    var productDataURL = "/api/json/total/" + productName + "/" + productMonth;
    res.view("total/overview", {dataURL: productDataURL, steps: steps});
  },

  productMonthWeek: function(req, res) {

    var productName = req.param("product");
    var productMonth = req.param("month");
    var productWeek = req.param("week");

    var steps = [
      {active: "active", title: "總覽"},
      {active: "active", title: productName},
      {active: "active", title: productMonth},
      {active: "active", title: "第 " + productWeek + " 週"},
      {active: "", title: "日期"},
      {active: "", title: "機器"}
    ];


    var productDataURL = "/api/json/total/" + productName + "/" + productMonth + "/" + productWeek;
    res.view("total/overview", {dataURL: productDataURL, steps: steps});
  },

  productMonthWeekDate: function(req, res) {

    var productName = req.param("product");
    var productMonth = req.param("month");
    var productWeek = req.param("week");
    var productDate = req.param("date");

    var steps = [
      {active: "active", title: "總覽"},
      {active: "active", title: productName},
      {active: "active", title: productMonth},
      {active: "active", title: "第 " + productWeek + " 週"},
      {active: "active", title: productDate + " 日"},
      {active: "", title: "機器"}
    ];

    var productDataURL = "/api/json/total/" + productName + "/" + productMonth + 
                         "/" + productWeek + "/" + productDate;

    res.view("total/overview", {dataURL: productDataURL, steps: steps});
  },

  productMonthWeekDateMachine: function(req, res) {

    var productName = req.param("product");
    var productMonth = req.param("month");
    var productWeek = req.param("week");
    var productDate = req.param("date");
    var productMachine = req.param("machine");

    var steps = [
      {active: "active", title: "總覽"},
      {active: "active", title: productName},
      {active: "active", title: productMonth},
      {active: "active", title: "第 " + productWeek + " 週"},
      {active: "active", title: productDate + " 日"},
      {active: "active", title: productMachine }
    ];

    var productDataURL = "/api/json/total/" + productName + "/" + productMonth + 
                         "/" + productWeek + "/" + productDate + "/" + productMachine;

    var year = req.param("month").split("-")[0];
    var month = +(req.param("month").split("-")[1]) - 1;

    res.view("total/machine", {
      fullYear: year,
      month: month,
      date: productDate,
      productName: productName,
      productMachine: productMachine,
      dataURL: productDataURL,
      steps: steps
    });
  }




};

