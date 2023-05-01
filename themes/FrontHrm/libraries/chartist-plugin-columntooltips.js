(function (root, factory) {
  if (typeof define === 'function' && define.amd) {
    // AMD. Register as an anonymous module unless amdModuleId is set
    define([], function () {
      return (root['Chartist.plugins.ctColumnTooltips'] = factory());
    });
  } else if (typeof exports === 'object') {
    // Node. Does not work with strict CommonJS, but
    // only CommonJS-like environments that support module.exports,
    // like Node.
    module.exports = factory();
  } else {
    root['Chartist.plugins.ctColumnTooltips'] = factory();
  }
}(this, function () {

/**
 * Chartist.js plugin to display x-axis culumn's summary tooltip
 * @author THE STORY <tell@thestory.pl>
 * @license MIT
 * @version 1.0.0
 */

/* global Chartist */
(function(window, document, Chartist) {
  'use strict';

  var defaultOptions = {

    currency: undefined,
    /**
     * Class names used by the plugin
     * @type {Object}
     */
    classNames: {
      /** Decoration line */
      line: 'ct-line',

      /** Decoration line group*/
      lineGroup: 'ct-lines',

      /** Hover column */
      column: 'ct-column',

      /** Hover column group */
      columnGroup: 'ct-columns',

      /** Hover column point */
      point: 'ct-custom-point',

      /** Hover column point group */
      pointGroup: 'ct-column-point-group',

      /** Hover column point groups */
      pointGroups: 'ct-column-point-groups',

      /** Tooltip */
      tooltip: 'ct-column-tooltip',

      /** Tooltip cloud */
      cloud: 'ct-cloud',
    },

    /**
     * Show decorative line on column hover
     * @type {Boolean}
     * @defaultvalue
     */
    showLine: true,

    /**
     * Show points on column hover - needs showPoint option to be true
     * @type {Boolean}
     * @defaultvalue
     */
    showPointsOnHover: true,
  };

  Chartist.plugins = Chartist.plugins || {};

  Chartist.plugins.ctColumnTooltips = function(options) {

    options = Chartist.extend({}, defaultOptions, options);

    return function ctColumnTooltips(chart) {

      /** @type {HTMLElement} Chart DOM object */
      var $chart = chart.container;

      /** @type {HTMLElement} Tooltip DOM object */
      var $tooltip = $chart.querySelector('.' + options.classNames.tooltip);

      /**
       * Alias to plugins objects container
       * @type {Object}
       */
      chart.ctColumnTooltips = {};

      // Create tooltip is there isn't any
      if (!$tooltip) {
        $tooltip = renderTooltip();
      }

      /**
       * Generates a tooltip
       * @return {HTMLElement} Tooltip DOM object
       */
      function renderTooltip() {
        var $tooltip = document.createElement('div');
        var $cloud = document.createElement('div');
        var $list = document.createElement('ul');

        $tooltip.classList.add(options.classNames.tooltip);
        $cloud.classList.add(options.classNames.cloud);

        for (var i = 0; i < chart.data.series.length; i++) {
          var $listItem = document.createElement('li');
          $list.appendChild($listItem);
        }
        $cloud.appendChild($list);
        $tooltip.appendChild($cloud);
        $chart.appendChild($tooltip);

        return $tooltip;
      }

      /**
       * Callback that handles the events
       *
       * @callback eventCallback
       * @param {Object} e Event object
       */

      /**
       * Sets event on charts certain children
       * @param {string}   event    Event name
       * @param {string}   selector Target elemnt's class
       * @param {eventCallback} callback The calllback that handles the event
       */
      function on(event, selector, callback) {
        $chart.addEventListener(event, function(e) {
          if (!selector || e.target.classList.contains(selector))
            callback(e);
        });
      }

      /**
       * Renders hover colums and their decorations
       * @param {Object} e   Event object
       */
      function renderColumns(e) {
        var step = getStep(e);

        if (options.showPointsOnHover) {
          chart.ctColumnTooltips.points = renderPoints(e, step);
        }

        if (options.showLine) {
          chart.ctColumnTooltips.lines = renderLines(e, step);
        }

        chart.ctColumnTooltips.columnSet = renderHoverColumns(e, step);
      }

      /**
       * Get the x-axis distance between set points
       * @param   {Object} e Event
       * @return  {number}   Distance between set points
       */
      function getStep(e) {
        var divider = chart.data.labels.length - chart.options.fullWidth;
        return e.chartRect.width() / divider;
      }

      /**
       * Generates points that will activate on column hover
       * @param   {Object} e Event object
       * @return  {Object}   SVG point group
       */
      function renderPoints(e, step) {
        var points = chart.svg.elem('g').addClass(options.classNames.pointGroups);
        points.points = [];

        for (var g = 0; g < chart.data.labels.length; g++) {
          var pointGroup = points.elem('g').addClass(options.classNames.pointGroup);
          points.points.push(pointGroup);
        }

        for (var i = 0; i < chart.ctColumnTooltips.oldPoints.length; i++) {
          for (var j = 0; j < chart.ctColumnTooltips.oldPoints[i].length; j++) {
            renderPoint(points, i, j);
          }
        }

        return points;
      }

      /**
       * Generates column point
       * @param  {Object} points [description]
       * @param  {number} i      [description]
       * @param  {number} j      [description]
       */
      function renderPoint(points, i, j) {
        var group = points.points[j];
        var point = group.elem('g').addClass(options.classNames.point);
        var oldPoint = chart.ctColumnTooltips.oldPoints[i][j];
        var positions = {
          x1: oldPoint._node.x1.baseVal.value,
          x2: oldPoint._node.x2.baseVal.value,
          y1: oldPoint._node.y1.baseVal.value,
          y2: oldPoint._node.y2.baseVal.value
        };
        point.elem('line', positions, 'ct-custom-point-shadow');
        point.elem('line', positions, 'ct-custom-point-fill');
        oldPoint.remove();
      }

      /**
       * Draws lines for column decoration
       * @param   {Object} e     Event object
       * @param   {number} step Distance between lines
       * @return  {Object}       SVG lines group
       */
      function renderLines(e, step) {
        var lines = chart.svg.elem('g').addClass(options.classNames.lineGroup);
        lines.lines = [];

        for (var i = 0; i < chart.data.labels.length; i++) {
          var xPos = step * i + e.axisX.chartRect.x1;
          var positions = {
            x1: xPos,
            x2: xPos,
            y1: e.chartRect.y1,
            y2: e.chartRect.y2
          };
          var line = lines.elem('line', positions, options.classNames.line);
          lines.lines.push(line);
        }

        return lines;
      }

      /**
       * Draws opaque columns used to call hover event
       * @param   {Object} e     Event object
       * @param   {number} step Distance between column centers
       * @return  {Object}       SVG hover column group ("<g>")
       */
      function renderHoverColumns(e, step) {
        var columns = chart.svg.elem('g').addClass(options.classNames.columnGroup);
        columns.columns = [];

        for (var i = 0; i < chart.data.labels.length; i++) {
          var xPos = step * i + e.axisX.chartRect.x1;
          var positions = {
            x1: xPos,
            x2: xPos,
            y1: e.chartRect.y1,
            y2: e.chartRect.y2
          };
          var column = columns.elem('line', positions, options.classNames.column).attr({
            'index': i,
            'stroke-width': step + 'px',
          });
          columns.columns.push(column);
        }

        return columns;
      }

      /**
       * Updates tooltip values with correspoinding column point values
       * @param {HTMLElement} $tooltip Tooltip item
       * @param {number} Index Column index
       */
      function updateTooltipValues($tooltip, index) {
        var $listItems = $tooltip.querySelectorAll('ul li');
        for (var i = 0; i < chart.data.series.length; i++) {
          var val = chart.data.series[i][index];

          if(options.currency)
            val = options.currency + val.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");

          $listItems[i].textContent = val;
        }
      }

      /**
       * Shows tooltip
       * @param {HTMLElement} $tooltip Tooltip DOM element
       * @param {number} posX Tooltip position
       */
      function showTooltip($tooltip, posX) {
        $tooltip.style.left = posX + 'px';
        $tooltip.classList.add('visible');

        if (posX > chart.ctColumnTooltips.columnSet.width() / 2) {
          $tooltip.classList.add('switch-side');
        } else {
          $tooltip.classList.remove('switch-side');
        }
      }

      /**
       * Shows decoration line on certain column index
       * @param {number} index Index of the colum where the line is located
       */
      function showLine(index) {
        chart.ctColumnTooltips.lines.lines[index].attr({
          'visible': true
        });
      }

      /**
       * Hides the certain column's decoration line
       * @param {number} index Index of the colum where the line is located
       */
      function hideLine(index) {
        chart.ctColumnTooltips.lines.lines[index].attr({
          'visible': false
        });
      }

      /**
       * Shows points on certain column index
       * @param {number} index Index of the colum where the points are located
       */
      function showPoints(index) {
        chart.ctColumnTooltips.points.points[index].attr({
          'visible': true
        });
      }

      /**
       * Hides column points
       * @param {index} index Column index where the points are located
       */
      function hidePoints(index) {
        chart.ctColumnTooltips.points.points[index].attr({
          'visible': false
        });
      }

      /**
       * Prepairs empty array for points
       */
      function preparePointData() {
        chart.ctColumnTooltips.oldPoints = [];
        for (var i = 0; i < chart.data.series.length; i++) {
          chart.ctColumnTooltips.oldPoints[i] = [];
        }
      }

      // Events

      on('mouseover', options.classNames.column, function(e) {
        /** @type {number} Hovered column's index */
        var index = e.target.getAttribute('index');

        if(chart.data.hidden_labels[index])
          $("div.ct-cloud ul").append("<center class='labelItem' style='border-top:1px solid #fff; white-space: nowrap;'>"+chart.data.hidden_labels[index]+"</center>");
        
        updateTooltipValues($tooltip, index);
        showTooltip($tooltip, e.target.attributes.x1.value);

        if (options.showLine) {
          showLine(index);
        }

        if (options.showPointsOnHover) {
          showPoints(index);
        }
      });

      on('mouseout', options.classNames.column, function(e) {
        /** @type {number} Hovered column's index */
        var index = e.target.getAttribute('index');

        $tooltip.classList.remove('visible');
        $("div.ct-cloud ul").children('.labelItem').remove();

        if (options.showLine) {
          hideLine(index);
        }

        if (options.showPointsOnHover) {
          hidePoints(index);
        }
      });

      if (options.showPointsOnHover) {
        if (chart.options.showPoint) {
          preparePointData();

          chart.on('draw', function(e) {
            if (e.type === 'point') {
              chart.ctColumnTooltips.oldPoints[e.seriesIndex][e.index] = e.element;
            }
          });
        } else {
          options.showPointsOnHover = false;
          console.warn("Chart's option 'showPoint' must be enabled touse hover points feature");
        }
      }

      chart.on('created', function(e) {
        renderColumns(e);
      });

      chart.on('update', function(e) {
        renderColumns(e);
      });
    };
  };

}(window, document, Chartist));

return Chartist.plugins.ctColumnTooltips;

}));