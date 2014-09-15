/**
 * DashboardController
 *
 * @description :: Server-side logic for managing users
 * @help        :: See http://links.sailsjs.org/docs/controllers
 */

module.exports = {

  index: function(req, res) {
    res.view("index");
  },

  main: function(req, res) {
    res.view("dashboard");
  },

  totalOverviewJSON: function(req, res) {
    Log.overviewByOrderType(res, function(result) {
      var resultJSON = {dataSet: result}
      res.json(resultJSON);
    });
  },

  totalProductJSON: function(req, res) {
    var product = req.param("product");

    Log.totalProduct(product, function(err, data) {

      if (err) {
        res.serverError(err);
      }

      var resultJSON = {
        steps: [product],
        dataSet: data
      }
      res.json(resultJSON);
    });
  },

  totalProductMonthJSON: function(req, res) {
    var product = req.param("product");
    var month = req.param("month")

    Log.totalProductMonth(product, month, function(err, data) {

      if (err) {
        res.serverError(err);
      }

      var resultJSON = {
        steps: [product, month],
        dataSet: data
      }

      res.json(resultJSON);
    });
  },

  totalProductMonthWeekJSON: function(req, res) {
    var product = req.param("product");
    var month = req.param("month");
    var week = req.param("week");

    Log.totalProductMonthWeek(product, month, week, function(err, data) {

      if (err) {
        res.serverError(err);
      }

      var resultJSON = {
        steps: [product, month, "第 " + week + " 週"],
        dataSet: data
      }

      res.json(resultJSON);
    });
  },

  totalProductMonthWeekDateJSON: function(req, res) {
    var product = req.param("product");
    var month = req.param("month");
    var week = req.param("week");
    var date = req.param("date");

    Log.totalProductMonthWeekDate(product, month, week, date, function(err, data) {

      if (err) {
        res.serverError(err);
      }

      var resultJSON = {
        steps: [product, month, "第 " + week + " 週", date + " 日"],
        dataSet: data
      }

      res.json(resultJSON);
    });
  }


};

