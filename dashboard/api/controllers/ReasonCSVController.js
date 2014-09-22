/**
 * DashboardController
 *
 * @description :: Server-side logic for managing users
 * @help        :: See http://links.sailsjs.org/docs/controllers
 */

module.exports = {

  overview: function(req, res) {

    var logReason = LogReason.jsonAPI();

    logReason.overview(function(err, result) {

      if (err) {
        res.serverError(err);
        return;
      }

      var lines = '"錯誤種類","數量"\n'

      res.type('text/csv');
      for (var i = 0; i < result.length; i++) {
        var record = result[i];
        lines += '"' + record.name + '",' + record.value + "\n";
      }

      res.end(lines);

    });
  },

  detailTable: function(req, res) {

    var logReason = LogReason.jsonAPI();
    var reasonID = req.param("reasonID");

    logReason.detailTable(reasonID, function(err, result) {

      if (err) {
        res.serverError(err);
        return;
      }

      var lines = '"日期","機器","數量"\n'

      res.type('text/csv');
      for (var i = 0; i < result.length; i++) {
        var record = result[i];
        lines += '"' + record.time + '", "' + record.name +'",' +  record.value + "\n";
      }

      res.end(lines);

    });
  }


};
