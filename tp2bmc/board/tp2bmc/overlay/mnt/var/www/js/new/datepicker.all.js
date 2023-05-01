/**
 * author:丘耀帆
 * github:https://github.com/qiuyaofan
 */
$(function () {
  /*==============BEGIN API============*/
  var defaultOptions = {
    min: false,
    max: false,
    format: 'YYYY-MM-DD HH:mm:ss',
    isRange: false,
    hasShortcut: false,
    shortcutOptions: [],
    // between:数字：30，string:month/year
    between: false,
    hide:function(){},
    show:function(){}
  };

  var API = {
    // 时分秒格式正则
    timeReg: function (_this) {
      var format = _this.config.format.split(' ')[1];
      var regText = format.replace(/HH/, '([0-9]{1,2})').replace(/:/g, '(:?)').replace(/(mm|ss)/g, '([0-9]{1,2})');
      return new RegExp('^' + regText + '$');
    },
    // 日期格式正则
    dayReg: function (_this) {
      var format = _this.config.format.split(' ')[0];
      var splitStrReg = new RegExp(_this.splitStr, 'g');
      var regText = format.replace(/YYYY/, '([1-9]{1}[0-9]{3})').replace(splitStrReg, '(' + _this.splitStr + '?)').replace(/(MM|DD)/g, '([0-9]{1,2})');
      return new RegExp('^' + regText + '$');
    },
    fixedFill: function (dayResult) {
      // 兼容201808变为2018-00-08的情况
      if (dayResult[3] == 0) {
        dayResult[3] = dayResult[5];
        dayResult[5] = '01';
      }
      if (dayResult[3].length == 1 && dayResult[5] == 0) {
        dayResult[3] = dayResult[3] + '0';
        dayResult[5] = '01';
      }
      if (dayResult[3].length == 2 && dayResult[5] == 0) {
        dayResult[5] = '01';
      }
      return dayResult;
    },
    // 日获取月份天数
    getMonthDay: function (month, year) {
      var prevMonth = month - 1 < 0 ? 11 : month - 1;
      return month === 2 && year % 4 === 0 ? '29' : EVERYMONTHHASDAY[prevMonth];
    },
    // 匹配是否有相应的时间格式
    getFormat: function (format) {
      var arr = ['YYYY', 'MM', 'DD', 'HH', 'mm', 'ss'];
      var result = [];
      for (var i = 0; i < arr.length; i++) {
        result.push(format.indexOf(arr[i]) !== -1);
      }
      return result;
    },
    // 获取原型实例对象
    getPicker: function ($el, type) {
      type = type || 'picker';
      return $el.parents('.c-datepicker-picker').data(type);
    },
    // 补0
    fillTime: function (val) {
      return Number(val) < 10 ? '0' + Number(val) : val;
    },
    // 修复月份超过最大最小
    fillMonth: function (month, year) {
      if (month < 1) {
        month = 12;
        year = year - 1;
      } else if (month > 12) {
        month = 1;
        year = year + 1;
      }
      return {
        month: month,
        year: year
      }
    },
    // 获取年月日
    getTimeFormat: function (_moment) {
      return {
        year: _moment.year(),
        month: _moment.month() + 1,
        day: _moment.date()
      }
    },
    newDateFixed: function (_this,temp){
      var reg = new RegExp(_this.splitStr,'g');
      return temp?new Date(temp.replace(reg,'/')):new Date();
    },
    // 范围获取具体年月日
    getRangeTimeFormat: function (_this,$input) {
      var temp;
      var val = $input.val();
      temp = val;
      var result = temp ? moment(API.newDateFixed(_this,temp)) : moment();
      return API.getTimeFormat(result);
    },
    // 判断最大值最小值
    minMaxFill: function (_this, result, index, type) {
      // 填充值
      var val;
      if (type === 'month') {
        val = result.year + _this.splitStr + API.fillTime(result.month);
      } else if (type === 'year') {
        val = result.year + '';
      } else {
        val = result.year + _this.splitStr + API.fillTime(result.month) + _this.splitStr + API.fillTime(result.day);
      }
      if (_this.hasTime) {
        val += ' ' + _this.$container.find('.c-datePicker__input-time').eq(index).val();
      }
      if (!_this.config.min && !_this.config.max) {
        val = val.split(' ')[0];
        return {
          val: val,
          result: result
        };
      }
      // 判断最大值最小值
      var nowMoment = moment(API.newDateFixed(_this,val));
      var minMoment = moment(API.newDateFixed(_this,_this.config.min));
      var maxMoment = moment(API.newDateFixed(_this,_this.config.max));
      // 不在范围内
      var isBefore = nowMoment.isBefore(minMoment);
      var isAfter = nowMoment.isAfter(maxMoment);
      if (isBefore && _this.config.min) {
        val = minMoment.format(_this.config.format).split(' ')[0];
        result = API.getTimeFormat(minMoment);
      }
      if (isAfter && _this.config.max) {
        val = maxMoment.format(_this.config.format).split(' ')[0];
        result = API.getTimeFormat(maxMoment);
      }
      val = val.split(' ')[0];
      return {
        val: val,
        result: result
      }
    },
    // 时分秒最大值检测
    timeCheck: function (time) {
      var arr = time.split(':');
      var checkArr = [23, 59, 59];
      for (var i = 0; i < arr.length; i++) {
        if (Number(arr[i]) > checkArr[i]) {
          arr[i] = checkArr[i];
        }
      }
      return arr.join(':');
    },
    // 超过最大
    maxMonth: function (val) {
      return val > 12;
    },
    // 小于最小
    minMonth: function (val) {
      return val < 1;
    },
    // 时分秒范围检测
    judgeTimeRange: function (_this, $day, $time, index) {
      index = index || 0;
      var day = $day.val();
      var time = $time.val();
      if (!day) {
        return;
      }
      var val = day + ' ' + time;
      var _moment = moment(API.newDateFixed(_this,val));
      var isBefore = _this.config.min ? _moment.isBefore((API.newDateFixed(_this, _this.config.min))) : false;
      var isAfter = _this.config.max ? _moment.isAfter((API.newDateFixed(_this, _this.config.max))) : false;
      if (!isBefore && !isAfter) {
        return;
      }
      if (isBefore && _this.config.min) {
        val = _this.config.min;
        $time.val(val.split(' ')[1]);
      } else if (isAfter && _this.config.max) {
        val = _this.config.max;
        $time.val(val.split(' ')[1]);
      }
      if (!_this.config.isRange) {
        _this.$input.eq(index).val(val);
      }
    },
    timeVal: function (_this, type) {
      var timeFormat = _this.config.format.split(' ')[1];
      return type === 'min' ? timeFormat.replace(/HH/, '00').replace(/mm/, '00').replace(/ss/, '00') : timeFormat.replace(/HH/, '23').replace(/mm/, '59').replace(/ss/, '59');
    },
    getScrollBarWidth: function () {
      var inner = document.createElement('p');
      inner.style.width = "100%";
      inner.style.height = "200px";
      var outer = document.createElement('div');
      outer.style.position = "absolute";
      outer.style.top = "0px";
      outer.style.left = "0px";
      outer.style.visibility = "hidden";
      outer.style.width = "200px";
      outer.style.height = "150px";
      outer.style.overflow = "hidden";
      outer.appendChild(inner);
      document.body.appendChild(outer);
      var w1 = inner.offsetWidth;
      outer.style.overflow = 'scroll';
      var w2 = inner.offsetWidth;
      if (w1 == w2) w2 = outer.clientWidth;
      document.body.removeChild(outer);
      var barWidth = w1 - w2;
      if (barWidth === 0) {
        barWidth = 15;
      }
      return barWidth;
    }
  };
  var JQTABLESCROLLWIDTH = API.getScrollBarWidth();

  var TEARTPL = '<table class="{{class}}" style="">' +
    '<tbody>' +
    '{{body}}' +
    '</tbody>' +
    '</table>';
  var TDTPL = '<td class="{{today}}">' +
    '<div>' +
    '<a class="cell">{{value}}</a>' +
    '</div>' +
    '</td>';
  var DAYHEADER = '<tr>' +
    '<th>日</th>' +
    '<th>一</th>' +
    '<th>二</th>' +
    '<th>三</th>' +
    '<th>四</th>' +
    '<th>五</th>' +
    '<th>六</th>' +
    '</tr>';
  var TIMELITPL = '<li class="c-datepicker-time-spinner__item {{className}}">{{time}}</li>';
  var TIMETPL = '<div class="c-datepicker-time-spinner__wrapper c-datepicker-scrollbar">' +
    '<div class="c-datepicker-scrollbar__wrap {{className}}" style="max-height: inherit; margin-bottom: -' + JQTABLESCROLLWIDTH + 'px; margin-right: -' + JQTABLESCROLLWIDTH + 'px;">' +
    '<ul class="c-datepicker-scrollbar__view c-datepicker-time-spinner__list">' +
    '{{li}}' +
    '</ul>' +
    '</div>' +
    '</div>';
  var TIMEMAINTPL = '<div class="c-datepicker-time-panel c-datepicker-popper" style="">' +
    '<div class="c-datepicker-time-panel__content has-seconds">' +
    '<div class="c-datepicker-time-spinner has-seconds">' +
    '{{time}}' +
    '</div>' +
    '</div>' +
    '<div class="c-datepicker-time-panel__footer">' +
    '<button type="button" class="c-datepicker-time-panel__btn min">0点</button>' +
    '<button type="button" class="c-datepicker-time-panel__btn max">23:59</button>' +
    '<button type="button" class="c-datepicker-time-panel__btn cancel">取消</button>' +
    '<button type="button" class="c-datepicker-time-panel__btn confirm">确定</button>' +
    '</div>' +
    '</div>';
  var SIDEBARBUTTON = '<button type="button" class="c-datepicker-picker__shortcut" data-value="{{day}}" data-time="{{time}}">{{name}}</button>';
  var SIDEBARTPL = '<div class="c-datepicker-picker__sidebar">' +
    '{{button}}' +
    '</div>';
  var PICKERFOOTERTPL = '<div class="c-datepicker-picker__footer" style="">' +
    '<button type="button" class="c-datepicker-button c-datepicker-picker__link-btn c-datepicker-button--text c-datepicker-button--mini {{className}}">' +
    '<span>' +
    '{{text}}' +
    '</span>' +
    '</button>' +
    '<button type="button" class="c-datepicker-button c-datepicker-picker__link-btn confirm c-datepicker-button--default c-datepicker-button--mini is-plain">' +
    '<span>' +
    '确定' +
    '</span>' +
    '</button>' +
    '</div>';
  var PICKERARROWTPL = '<div x-arrow="" class="popper__arrow" style="left: 35px;"></div>';
  var PICKERHEADERTPL = '<div class="{{className}}__header">' +
    '{{prev}}' +
    '<span role="button" class="{{className}}__header-label {{className}}__header-year"><span>{{year}}</span> 年</span>' +
    '<span role="button" class="{{className}}__header-label {{className}}__header-month"><span>{{month}}</span> 月</span>' +
    '{{next}}' +
    '</div>';
  PICKERHEADERPREVTPL = '<i class="kxiconfont icon-first c-datepicker-picker__icon-btn {{className}}__prev-btn year" aria-label="前一年"></i>' +
    '<i class="kxiconfont icon-left c-datepicker-picker__icon-btn {{className}}__prev-btn month" aria-label="上个月"></i>';
  PICKERHEADERNEXTTPL = '<i class="kxiconfont icon-right c-datepicker-picker__icon-btn {{className}}__next-btn month" aria-label="下个月"></i>' +
    '<i class="kxiconfont icon-last c-datepicker-picker__icon-btn {{className}}__next-btn year" aria-label="后一年"></i>';
  PICKERHEADERNEXTSINGLETPL = '<i class="kxiconfont icon-last c-datepicker-picker__icon-btn {{className}}__next-btn year" aria-label="后一年"></i>' +
    '<i class="kxiconfont icon-right c-datepicker-picker__icon-btn {{className}}__next-btn month" aria-label="下个月"></i>';
  var PICKERTIMEHEADERTPL = '<span class="{{className}}__editor-wrap">' +
    '<div class="c-datepicker-input c-datepicker-input--small">' +
    '<input type="text" autocomplete="off" placeholder="选择日期" class="c-datepicker-input__inner c-datePicker__input-day">' +
    '</div>' +
    '</span>' +
    '<span class="{{className}}__editor-wrap">' +
    '<div class="c-datepicker-input c-datepicker-input--small">' +
    '<input type="text" autocomplete="off" placeholder="选择时间" class="c-datepicker-input__inner c-datePicker__input-time">' +
    '</div>' +
    '</span>';
  //  范围
  var RANGEPICKERMAINTPL = '<div class="c-datepicker-picker c-datepicker-date-range-picker c-datepicker-popper {{hasTime}} {{hasSidebar}}" x-placement="top-start">' +
    '<div class="c-datepicker-picker__body-wrapper">' +
    '{{sidebar}}' +
    '<div class="c-datepicker-picker__body">' +
    '<div class="c-datepicker-date-range-picker__time-header">' +
    '<div class="c-datepicker-date-range-picker__time-content">' +
    PICKERTIMEHEADERTPL.replace(/{{className}}/g, 'c-datepicker-date-range-picker') +
    '</div>' +
    '<span class="kxiconfont icon-right"></span>' +
    '<div class="c-datepicker-date-range-picker__time-content">' +
    PICKERTIMEHEADERTPL.replace(/{{className}}/g, 'c-datepicker-date-range-picker') +
    '</div>' +
    '</div>' +
    '<div class="c-datepicker-picker__body-content">' +
    '<div class="c-datepicker-date-range-picker-panel__wrap is-left">' +
    PICKERHEADERTPL.replace(/{{prev}}/g, PICKERHEADERPREVTPL).replace(/{{next}}/g, '').replace(/{{className}}/g, 'c-datepicker-date-range-picker') +
    '<div class="c-datepicker-picker__content">' +
    '{{table}}' +
    '</div>' +
    '</div>' +
    '<div class="c-datepicker-date-range-picker-panel__wrap is-right">' +
    PICKERHEADERTPL.replace(/{{prev}}/g, '').replace(/{{next}}/g, PICKERHEADERNEXTTPL).replace(/{{year}}/g, '{{yearEnd}}').replace(/{{month}}/g, '{{monthEnd}}').replace(/{{className}}/g, 'c-datepicker-date-range-picker') +
    '<div class="c-datepicker-picker__content">' +
    '{{table}}' +
    '</div>' +
    '</div>' +
    '</div>' +
    '</div>' +
    '</div>' +
    PICKERFOOTERTPL.replace(/{{className}}/g, 'c-datepicker-picker__btn-clear').replace(/{{text}}/g, '清空') +
    PICKERARROWTPL +
    '</div>';
  var PICKERFOOTERNOWBUTTON = PICKERFOOTERTPL.replace(/{{className}}/g, 'c-datepicker-picker__btn-now').replace(/{{text}}/g, '此刻');
  var PICKERFOOTERCLEARBUTTON = PICKERFOOTERTPL.replace(/{{className}}/g, 'c-datepicker-picker__btn-clear').replace(/{{text}}/g, '清空');
  // 单个
  var DATEPICKERMAINTPL = '<div class="c-datepicker-picker c-datepicker-date-picker c-datepicker-popper {{hasTime}} {{hasSidebar}}" x-placement="top-start">' +
    '<div class="c-datepicker-picker__body-wrapper">' +
    '{{sidebar}}' +
    '<div class="c-datepicker-picker__body">' +
    '<div class="c-datepicker-date-picker__time-header">' +
    PICKERTIMEHEADERTPL.replace(/{{className}}/g, 'c-datepicker-date-picker') +
    '</div>' +
    PICKERHEADERTPL.replace(/{{prev}}/g, PICKERHEADERPREVTPL).replace(/{{next}}/g, PICKERHEADERNEXTSINGLETPL).replace(/{{className}}/g, 'c-datepicker-date-picker') +
    ' <div class="c-datepicker-picker__content">' +
    '{{table}}' +
    '</div>' +
    '</div>' +
    '</div>' +
    '{{footerButton}}' +
    PICKERARROWTPL +
    '</div>';
  var EVERYMONTHHASDAY = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
  var MONTHWORDS = ['一月', '二月', '三月', '四月', '五月', '六月', '七月', '八月', '九月', '十月', '十一月', '十二月'];
  /*==============END API============*/

  /*===============BEGIN 发布订阅==================*/
  // 全局组件：pub/sub
  var o = $({});

  $.sub = function () {
    o.on.apply(o, arguments);
  };

  $.unsub = function () {
    o.off.apply(o, arguments);
  };

  $.pub = function () {
    o.trigger.apply(o, arguments);
  };
  /*===============END 发布订阅==================*/

  /*==============BEGIN YEAR============*/
  // .c-datepicker-picker__content
  function Year(picker) {
    this.picker = picker;
    this.init();
  }

  $.extend(Year.prototype, {
    init: function () {

    },
    event: function () {
      // 点击选择年
      if (!this.picker.config.isRange) {
        this.picker.$container.on('click', '.c-datepicker-year-table td.available', function () {
          if ($(this).hasClass('disabled')) {
            return;
          }
          var _this = API.getPicker($(this), 'year');
          var activeNum = $(this).text();
          _this.picker.$container.find('.c-datepicker-date-picker__header-year span').text(activeNum);
          // 年类型，无月、日期
          if (_this.picker.params.isYear) {
            _this.picker.$input.val(activeNum);
            _this.picker.$container.find('.c-datepicker-year-table td.current').removeClass('current');
            $(this).addClass('current');
            _this.picker.datePickerObject.hide();
          } else {
            _this.picker.monthObject.render();
            _this.hide();
          }
        })
      }
    },
    show: function () {
      this.picker.$container.find('.c-datepicker-date-table,.c-datepicker-month-table,.c-datepicker-date-picker__header-month').hide();
      this.picker.$container.find('.c-datepicker-year-table').show();
    },
    hide: function () {
      this.picker.$container.find('.c-datepicker-year-table').hide();
      this.picker.$container.find('.c-datepicker-date-picker__prev-btn.year,.c-datepicker-date-picker__next-btn.year').removeClass('is-year');
    },
    render: function (year) {
      var html = this.renderHtml(year);
      var $year = this.picker.$container.find('.c-datepicker-year-table');
      if (!$year.length) {
        this.picker.$container.find('.c-datepicker-picker__content').append(html);
        this.picker.$container.data('year', this);
        this.show();
        this.event();
      } else {
        $year.replaceWith(html);
        this.show();
      }
      this.picker.$container.find('.c-datepicker-date-picker__prev-btn.month,.c-datepicker-date-picker__next-btn.month').hide();
      this.picker.$container.find('.c-datepicker-date-picker__prev-btn.year,.c-datepicker-date-picker__next-btn.year').addClass('is-year');
    },
    renderHtml: function (year) {
      year = year || moment().year();
      var min = Number(parseInt(year / 10) + '0');
      // var max = min+9;
      var temp = '';
      var html = '';
      var val = this.picker.$input.val();
      var activeYear = val ? API.getTimeFormat(moment(API.newDateFixed(this.picker,val))).year : false;
      this.picker.$container.find('.c-datepicker-date-picker__header-year span').text(min + '年-' + (min + 9));
      for (var index = 0; index < 10; index++) {
        var _val = min + index;
        var className = _val == activeYear ? 'current available' : 'available';
        if (_val < this.picker.minJson.year || _val > this.picker.maxJson.year) {
          className += ' disabled';
        }
        temp += TDTPL.replace('{{value}}', _val).replace('{{today}}', className);
        if ((index + 1) % 4 === 0) {
          html += '<tr>' + temp + '</tr>';
          temp = '';
        }
      }
      if (temp) {
        html += '<tr>' + temp + '</tr>';
      }
      html = TEARTPL.replace('{{body}}', html).replace('{{class}}', 'c-datepicker-year-table');
      return html;
    }
  });


  /*==============END YEAR============*/

  /*==============BEGIN MONTH============*/
  // 月
  function Month(picker) {
    this.picker = picker;
    this.init();
  }

  $.extend(Month.prototype, {
    init: function () {

    },
    event: function () {
      if (!this.picker.config.isRange) {
        // 点击选择月份
        this.picker.$container.on('click', '.c-datepicker-month-table td.available', function () {
          if ($(this).hasClass('disabled')) {
            return;
          }
          var _this = API.getPicker($(this), 'month');
          var year = _this.picker.$container.find('.c-datepicker-date-picker__header-year span').text();
          var month = _this.picker.$container.find('.c-datepicker-month-table td').index($(this)) + 1;
          _this.picker.$container.find('.c-datepicker-date-picker__header-month span').text(month);
          // 是选择月，无日期
          if (_this.picker.params.isMonth) {
            var val = year + _this.picker.splitStr + API.fillTime(month);
            _this.picker.$input.val(val);
            _this.picker.$container.find('.c-datepicker-month-table td.current').removeClass('current');
            $(this).addClass('current');
            _this.picker.datePickerObject.hide();
          } else {
            _this.picker.dayObject.renderSingle(year, month, false, true);
            _this.hide();
          }
        })
      }
    },
    show: function () {
      this.picker.$container.find('.c-datepicker-month-table').show();
      this.picker.$container.find('.c-datepicker-date-table,.c-datepicker-year-table').hide();
    },
    hide: function () {
      this.picker.$container.find('.c-datepicker-date-picker__prev-btn.month,.c-datepicker-date-picker__next-btn.month').show();
      this.picker.$container.find('.c-datepicker-date-picker__header-month').show();
      this.picker.$container.find('.c-datepicker-month-table').hide();
      this.picker.$container.find('.c-datepicker-date-picker__prev-btn.year,.c-datepicker-date-picker__next-btn.year').removeClass('is-month');
    },
    render: function () {
      var html = this.renderHtml();
      var $month = this.picker.$container.find('.c-datepicker-month-table');
      if (!$month.length) {
        this.picker.$container.find('.c-datepicker-picker__content').append(html);
        this.picker.$container.data('month', this);
        this.show();
        this.event();
      } else {
        $month.replaceWith(html);
        this.show();
      }
      this.picker.$container.find('.c-datepicker-date-picker__prev-btn.year,.c-datepicker-date-picker__next-btn.year').addClass('is-month');
    },
    renderHtml: function () {
      // month = month || moment().month() + 1;
      var min = 1;
      var temp = '';
      var html = '';
      var nowYear = this.picker.$container.find('.c-datepicker-date-picker__header-year span').text();
      var minYear = this.picker.minJson.year;
      var maxYear = this.picker.maxJson.year;
      var disabledName = '';
      var isSame = false;
      // 不在范围内
      if (nowYear < minYear || nowYear > maxYear) {
        disabledName = ' disabled';
      } else if (nowYear == minYear || nowYear == maxYear) {
        isSame = true;
        var minMonth, maxMonth;
        if (maxYear == minYear) {
          minMonth = this.picker.minJson.month;
          maxMonth = this.picker.maxJson.month;
        } else if (nowYear == minYear) {
          minMonth = this.picker.minJson.month;
          maxMonth = 13;
        } else if (nowYear == maxYear) {
          minMonth = 0;
          maxMonth = this.picker.maxJson.month;
        }
      }
      var val = this.picker.$input.val();
      var formatResult = API.getTimeFormat(moment(API.newDateFixed(this.picker,val)));
      var activeMonth = val && (formatResult.year == nowYear) ? formatResult.month : false;
      for (var index = 0; index < 12; index++) {
        var _val = min + index;
        // var className = 'available';
        var className = _val === activeMonth ? 'current available' : 'available';
        className += disabledName;
        if (isSame && (_val < minMonth || _val > maxMonth)) {
          className += ' disabled';
        }
        temp += TDTPL.replace('{{value}}', MONTHWORDS[index]).replace('{{today}}', className);
        if ((index + 1) % 4 === 0) {
          html += '<tr>' + temp + '</tr>';
          temp = '';
        }
      }
      html = TEARTPL.replace('{{body}}', html).replace('{{class}}', 'c-datepicker-month-table');
      return html;
    }
  });

  /*==============END MONTH============*/

  /*==============BEGIN DAY============*/
  // 日期
  function Day(picker) {
    this.picker = picker;
    this.init();
  }

  $.extend(Day.prototype, {
    init: function () {
      this.current = 0;
    },
    eventSingle: function () {
      // 点击选择日期
      this.picker.$container.on('click', '.c-datepicker-date-table td.available', function (event) {
        event.stopPropagation();
        var $this=$(this);
        var _this = API.getPicker($this, 'day');
        if ($this.hasClass('disabled')) {
          return;
        }
        if (_this.picker.isBlur){
          var $wrap = $this.parents('.c-datepicker-picker__content');
          var index = $wrap.find('.c-datepicker-date-table td').index($this);
          $.sub('datapickerClick',function(e){
            $this = $wrap.find('.c-datepicker-date-table td').eq(index);
            clickDate(_this, $this);
            $.unsub('datapickerClick');
          });
          $.pub('datapickerRenderPicker');
        }else{
          clickDate(_this, $this);
        }
      })
      function clickDate(_this,$target){
        var activeNum = $target.text();
        _this.picker.$container.find('.c-datepicker-date-table td.current').removeClass('current');
        $target.addClass('current');
        var val = _this.picker.$container.find('.c-datePicker__input-day').val();
        if (!val) {
          var time = moment().format(_this.picker.config.format).split(' ')[1];
          _this.picker.$container.find('.c-datePicker__input-time').val(time);
          setValue.call(_this, activeNum, moment(), moment());
        } else {
          var inputVal = _this.picker.$input.val();
          setValue.call(_this, activeNum, moment(API.newDateFixed(_this.picker, val)), moment(API.newDateFixed(_this.picker, inputVal)));
        }
        if (!_this.picker.hasTime) {
          _this.picker.datePickerObject.hide();
        } else {
          API.judgeTimeRange(_this.picker, _this.picker.$container.find('.c-datePicker__input-day'), _this.picker.$container.find('.c-datePicker__input-time'));
        }
      }
      // 设置选中值
      function setValue(activeNum, input, inputDay) {
        var year = this.picker.$container.find('.c-datepicker-date-picker__header-year span').text();
        var month = this.picker.$container.find('.c-datepicker-date-picker__header-month span').text() - 1;
        // picker里的日期input
        val = input.set({ 'year': year, 'month': month, 'date': activeNum }).format(this.picker.config.format.split(' ')[0]);
        this.picker.$container.find('.c-datePicker__input-day').val(val);
        // 整个表单input update
        var inputVal = inputDay.set({ 'year': year, 'month': month, 'date': activeNum }).format(this.picker.config.format);
        this.picker.$input.val(inputVal);
      }
    },
    eventRange: function () {
      // 点击选择日期
      this.picker.$container.on('click', '.c-datepicker-date-table td.available', function (event) {
        event.stopPropagation();
        var $this = $(this);
        if ($this.hasClass('disabled')) {
          return;
        }
        var _this = API.getPicker($this, 'day');
        if (_this.picker.isBlur) {
          var $wrap = $this.parents('.c-datepicker-date-range-picker-panel__wrap');
          var index = $wrap.find('td').index($this);
          $.sub('datapickerClick', function (e) {
            $this = $wrap.find('td').eq(index);
            clickDateRange(_this, $this);
            $.unsub('datapickerClick');
          });
          $.pub('datapickerRenderPicker');
        } else {
          clickDateRange(_this, $this);
        }
      })

      function clickDateRange(_this,$target){
        // var _this = API.getPicker($(this), 'day');
        var $wrap = _this.picker.$container.find('.c-datepicker-date-range-picker-panel__wrap');
        $wrap.find('td.current.hover').removeClass('current hover');
        var $current = $wrap.find('td.current');

        var $activeWrap = $target.parents('.c-datepicker-date-range-picker-panel__wrap');
        var date = $target.find('.cell').text();
        var $day = _this.picker.$container.find('.c-datePicker__input-day');
        var $time = _this.picker.$container.find('.c-datePicker__input-time');
        var year = $activeWrap.find('.c-datepicker-date-range-picker__header-year span').text();
        var month = $activeWrap.find('.c-datepicker-date-range-picker__header-month span').text() - 1;
        // 重选
        if (_this.current >= 2) {
          $current.removeClass('current');
          $wrap.find('td.in-range').removeClass('in-range');
          $current = $wrap.find('td.current');
          _this.current = 0;
        }
        if (!_this.current) {
          $target.addClass('current');
          var inputVal = moment().set({ 'year': year, 'month': month, 'date': date }).format(_this.picker.config.format.split(' ')[0]);
          $day.val(inputVal);
          $time.eq(0).val(_this.picker.timeMin);
          $time.eq(1).val(_this.picker.timeMax);
          _this.current = 1;
        } else if (_this.current == 1) {
          // 选完两个
          $target.addClass('current');
          var existDate = $day.eq(0).val();
          var inputVal = moment().set({ 'year': year, 'month': month, 'date': date }).format(_this.picker.config.format.split(' ')[0]);
          var a = moment(API.newDateFixed(_this.picker, existDate));
          var b = moment(API.newDateFixed(_this.picker, inputVal));
          // 比初选的小，交换
          if (!_this.picker.hasTime) {
            // 没有十分秒，则选完就隐藏时间插件，并赋值输入框
            if (b.diff(a) < 0) {
              var temp = inputVal;
              inputVal = existDate;
              existDate = temp;
            }
            _this.current = 2;
            _this.picker.$inputBegin.val(existDate);
            _this.picker.$inputEnd.val(inputVal);
            _this.picker.datePickerObject.hide();
          } else {
            // 有十分秒，则选添加选择范围样式
            if (b.diff(a) < 0) {
              $day.eq(0).val(inputVal);
              $day.eq(1).val(existDate);
            } else {
              $day.eq(1).val(inputVal);
            }
            _this.current = 2;
            _this.addRangeClass();
          }
        }

        if (_this.current) {
          var index = _this.current - 1;
          API.judgeTimeRange(_this.picker, _this.picker.$container.find('.c-datePicker__input-day').eq(index), _this.picker.$container.find('.c-datePicker__input-time').eq(index), index);
        }
      }

      // 移动渲染范围模式，前提是只有一个current时
      this.picker.$container.on('mouseenter', '.c-datepicker-date-table td.available', function () {
        var _this = API.getPicker($(this), 'day');
        if (_this.current != 1) {
          return;
        }
        _this.picker.$container.find('td.current.hover').removeClass('current hover');
        $(this).addClass('current hover');
        var $wrap = $(this).parents('.c-datepicker-date-range-picker-panel__wrap');
        var $start = _this.picker.$container.find('.c-datePicker__input-day').eq(0);
        var year = $wrap.find('.c-datepicker-date-range-picker__header-year span').text();
        var month = $wrap.find('.c-datepicker-date-range-picker__header-month span').text();
        var date = $(this).find('.cell').text();
        var start = $start.val();
        var end = year + _this.picker.splitStr + month + _this.picker.splitStr + date;
        // 需要交换开始结束时间
        if (moment(API.newDateFixed(_this.picker,start)).isAfter(API.newDateFixed(_this.picker,end))) {
          var temp = start;
          start = end;
          end = temp;
        }
        _this.addRangeClass(moment(API.newDateFixed(_this.picker,start)), moment(API.newDateFixed(_this.picker,end)), true);
      })
    },
    show: function () {
      this.picker.$container.find('.c-datepicker-year-table,.c-datepicker-month-table').hide();
      this.picker.$container.find('.c-datepicker-date-table').show();
    },
    hide: function () {
      this.picker.$container.find('.c-datepicker-date-table').hide();
    },
    render: function (year, month, today, baseEnd, reRender) {
      if (this.picker.config.isRange) {
        this.renderRange(year, month, today, baseEnd, reRender);
      } else {
        this.renderSingle(year, month, today, reRender);
      }
    },
    renderSingle: function (year, month, today, reRender) {
      var html = this.renderHtml(year, month, today);
      var $year = this.picker.$container.find('.c-datepicker-date-table');
      if ($year.length && !reRender) {
        this.addCurrentSingle();
        this.show();
      } else {
        var $content = this.picker.$container.find('.c-datepicker-picker__content');
        // 日历头部
        var $year = this.picker.$container.find('.c-datepicker-date-picker__header-year span');
        var $month = this.picker.$container.find('.c-datepicker-date-picker__header-month span');
        $year.text(year);
        $month.text(month);
        if (!$content.find('.c-datepicker-date-table').length) {
          $content.append(html);
        } else {
          $content.find('.c-datepicker-date-table').replaceWith(html);
        }
        if (!this.picker.$container.data('day')) {
          this.picker.$container.data('day', this);
        }
        this.addCurrentSingle();
        this.show();
        if (!reRender){
          this.eventSingle();
        }
      }
    },
    addCurrentSingle: function () {
      var val = this.picker.$input.val();
      if (!val) {
        return;
      }
      if (!API.dayReg(this.picker).test(val.split(' ')[0])) {
        return;
      }
      var result = API.getTimeFormat(moment(API.newDateFixed(this.picker,val)));
      var year = this.picker.$container.find('.c-datepicker-date-picker__header-year span').text();
      var month = this.picker.$container.find('.c-datepicker-date-picker__header-month span').text();
      if (result.year == year && result.month == month) {
        var $day = this.picker.$container.find('.c-datepicker-date-table td.available');
        $day.removeClass('current');
        $day.eq(result.day - 1).addClass('current');
      }
    },
    // 显示和选中值有联动关系，和输入框修改日期有关
    renderRange: function (year, month, today, baseEnd, reRender) {
      var $year = this.picker.$container.find('.c-datepicker-date-table');
      if ($year.length && !reRender) {
        this.show();
      } else {
        var index = 0, distance = 1, countFn = API.maxMonth, initMonth = 1;
        if (baseEnd) {
          index = 1;
          distance = -1;
          countFn = API.minMonth;
          initMonth = 12;
        }
        // today[index]
        var html = this.renderHtml(year[index], month[index], false);
        // 选中间隔月份
        var monthEnd = month[index] + distance;
        var yearEnd = year[index];
        if (countFn(monthEnd)) {
          monthEnd = initMonth;
          yearEnd = yearEnd + distance;
        }
        var htmlEnd = this.renderHtml(yearEnd, monthEnd, false);

        // 日历头部
        var $year = this.picker.$container.find('.c-datepicker-date-range-picker__header-year');
        var $month = this.picker.$container.find('.c-datepicker-date-range-picker__header-month');
        $year.eq(index).find('span').text(year[index]);
        $month.eq(index).find('span').text(month[index]);
        $year.eq(1 - index).find('span').text(yearEnd);
        $month.eq(1 - index).find('span').text(monthEnd);

        this.picker.$container.find('.c-datepicker-picker__content').eq(index).html(html);
        this.picker.$container.find('.c-datepicker-picker__content').eq(1 - index).html(htmlEnd);
        if (!this.picker.$container.data('day')) {
          this.picker.$container.data('day', this);
        }
        this.addRangeClass();
        if (!reRender) {
          this.eventRange();
        }
      }
    },
    // 上一月下一月，上一年下一年修改显示界面
    prevNextSingle: function (moveType, type) {
      var $year = this.picker.$container.find('.c-datepicker-date-picker__header-year');
      var $month = this.picker.$container.find('.c-datepicker-date-picker__header-month');
      var year = Number($year.find('span').text());
      var month = Number($month.find('span').text());
      var day = this.picker.$container.find('.c-datePicker__input-day').val();
      var dayFormat = API.getTimeFormat(moment(API.newDateFixed(this.picker,day)));
      var count = 1;
      if (moveType === 'prev') {
        count = -1;
      }
      if (type === 'year') {
        year = year + count;
      } else if (type === 'month') {
        month = month + count;
        var result = API.fillMonth(month, year);
        month = result.month;
        year = result.year;
      }
      var date=false;
      if (dayFormat.year == year && dayFormat.month == month){
        date = dayFormat.day;
      }
      var html = this.renderHtml(year, month, date);
      // 日历头部
      $year.find('span').text(year);
      $month.find('span').text(month);
      var $content = this.picker.$container.find('.c-datepicker-picker__content');
      var $table = $content.find('.c-datepicker-date-table');
      this.picker.$container.find('.c-datepicker-month-table,.c-datepicker-year-table').hide();
      if ($table.length) {
        $table.replaceWith(html);
      } else {
        $content.append(html);
      }
    },
    // 上一月下一月，上一年下一年修改显示界面
    prevNextRender: function (moveType, type) {
      var $year = this.picker.$container.find('.c-datepicker-date-range-picker__header-year');
      var $month = this.picker.$container.find('.c-datepicker-date-range-picker__header-month');
      var year = Number($year.eq(0).find('span').text());
      var month = Number($month.eq(0).find('span').text());
      var count = 1;
      var endYear, endMonth;
      if (moveType === 'prev') {
        count = -1;
      }
      if (type === 'year') {
        year = year + count;
      } else if (type === 'month') {
        month = month + count;
      }
      var result = API.fillMonth(month, year);
      month = result.month;
      year = result.year;
      endMonth = month + 1;
      var endResult = API.fillMonth(endMonth, year);
      endMonth = endResult.month;
      endYear = endResult.year;

      var html = this.renderHtml(year, month, false);
      var htmlEnd = this.renderHtml(endYear, endMonth, false);
      // 日历头部
      $year.eq(0).find('span').text(year);
      $month.eq(0).find('span').text(month);
      $year.eq(1).find('span').text(endYear);
      $month.eq(1).find('span').text(endMonth);
      this.picker.$container.find('.c-datepicker-picker__content').eq(0).html(html);
      this.picker.$container.find('.c-datepicker-picker__content').eq(1).html(htmlEnd);
      this.addRangeClass(false, false, true);
    },
    // 拼接day的html
    renderHtml: function (year, month, activeDay) {
      var _moment = moment();
      month = month || _moment.month() + 1;
      year = year || _moment.year();
      var today = (_moment.month() + 1 === month) && (_moment.year() === year) ? _moment.date() : '';

      var prevMonthDay = API.getMonthDay(month - 1, year);
      var day = API.getMonthDay(month, year);
      var weekday = moment().set({ 'year': year, 'month': month - 1, 'date': 1 }).weekday();
      var weekdayLast = moment().set({ 'year': year, 'month': month - 1, 'date': day }).weekday();
      var html = DAYHEADER;
      var min = 1;
      var temp = '';
      var row = 0;
      // prev-month
      if (weekday != 0) {
        for (var prev = weekday - 1; prev >= 0; prev--) {
          var className = 'prev-month';
          temp += TDTPL.replace('{{value}}', prevMonthDay - prev).replace('{{today}}', className);
          if ((weekday - prev) % 7 === 0) {
            html += '<tr>' + temp + '</tr>';
            temp = '';
            row += 1;
          }
        }
      }
      var begin = weekday % 7;
      // active day 
      var hasMin = this.picker.minJson ? true : false;
      var hasMax = this.picker.maxJson ? true : false;
      var minMonth = hasMin ? moment(API.newDateFixed(this.picker, this.picker.minJson.year + this.picker.splitStr + this.picker.minJson.month + this.picker.splitStr + 1)) : false;
      var maxMonth = hasMax ? moment(API.newDateFixed(this.picker, this.picker.maxJson.year + this.picker.splitStr + this.picker.maxJson.month + this.picker.splitStr + 1)) : false;
      var disabledName = '';
      var isSame = false;
      var nowDate = moment(API.newDateFixed(this.picker, year + this.picker.splitStr + month + this.picker.splitStr + 1));
      // 不在范围内
      if ((hasMin && nowDate.isBefore(minMonth)) || (hasMax && nowDate.isAfter(maxMonth))) {
        disabledName = ' disabled';
      } else if ((hasMin && nowDate.isSame(minMonth)) || (hasMax && nowDate.isSame(maxMonth))) {
        isSame = true;
        var minDay, maxDay;
        if (hasMin && hasMax && maxMonth.isSame(minMonth)) {
          minDay = this.picker.minJson.day;
          maxDay = this.picker.maxJson.day;
        } else if (hasMin && nowDate.isSame(minMonth)) {
          minDay = this.picker.minJson.day;
          maxDay = 32;
        } else if (hasMax && nowDate.isSame(maxMonth)) {
          minDay = 0;
          maxDay = this.picker.maxJson.day;
        }
      }
      for (var index = 0; index < day; index++) {
        var className = 'available' + disabledName;
        var _val = min + index;
        if (_val === today) {
          className += ' today';
        }
        if (_val === activeDay) {
          className += ' current';
        }
        if (isSame && (_val < minDay || _val > maxDay)) {
          className += ' disabled';
        }
        // var className = _val === today ? _val === selectedDate ? 'current today available' : 'available';
        temp += TDTPL.replace('{{value}}', _val).replace('{{today}}', className);
        if ((begin + index + 1) % 7 === 0) {
          html += '<tr>' + temp + '</tr>';
          temp = '';
          // 兼容刚好换行的bug
          if (index != (day - 1)) {
            row += 1;
          }
        }
      }
      begin = (weekday + day) % 7;
      // next-month
      var nextMax = (6 - row - 1) * 7 + (6 - weekdayLast);
      for (var next = 0; next < nextMax; next++) {
        var className = 'next-month';
        temp += TDTPL.replace('{{value}}', 1 + next).replace('{{today}}', className);
        if ((begin + next + 1) % 7 === 0) {
          html += '<tr>' + temp + '</tr>';
          temp = '';
        }
      }

      html = TEARTPL.replace('{{body}}', html).replace('{{class}}', 'c-datepicker-date-table');
      return html;
    },
    // 添加时间范围类名
    addRangeClass: function (defaultStart, defaultEnd, isHover) {
      var $wrap = this.picker.$container.find('.c-datepicker-date-range-picker-panel__wrap');
      $wrap.find('td.available').removeClass('in-range start-date end-date');
      var $days = this.picker.$container.find('.c-datePicker__input-day');
      var $years = this.picker.$container.find('.c-datepicker-date-range-picker__header-year');
      var $months = this.picker.$container.find('.c-datepicker-date-range-picker__header-month');
      var start = defaultStart || $days.eq(0).val();
      var end = defaultEnd || $days.eq(1).val();

      if (!start || !end) {
        return;
      }
      // 不是hover的时候
      if (!isHover) {
        this.current = 2;
      }
      var startMoment = defaultStart || moment(API.newDateFixed(this.picker,$days.eq(0).val()));
      var endMoment = defaultEnd || moment(API.newDateFixed(this.picker,$days.eq(1).val()));
      var startYear = $years.eq(0).find('span').text();
      var endYear = $years.eq(1).find('span').text();
      var startMonth = $months.eq(0).find('span').text();
      var endMonth = $months.eq(1).find('span').text();
      var startRangeDate = startYear + this.picker.splitStr + startMonth + this.picker.splitStr + 1;
      var endRangeDate = endYear + this.picker.splitStr + endMonth + this.picker.splitStr + API.getMonthDay(endMonth, endYear);
      var isStartBetween = !(startMoment.isBefore(API.newDateFixed(this.picker,startRangeDate)) || startMoment.isAfter(API.newDateFixed(this.picker,endRangeDate)));;
      var isEndBetween = !(endMoment.isBefore(API.newDateFixed(this.picker,startRangeDate)) || endMoment.isAfter(API.newDateFixed(this.picker,endRangeDate)));
      var index;
      // 所选值不在显示范围内
      var isBefore = startMoment.isBefore(API.newDateFixed(this.picker,startRangeDate)) && startMoment.isBefore(API.newDateFixed(this.picker,endRangeDate)) && endMoment.isBefore(API.newDateFixed(this.picker,startRangeDate)) && endMoment.isBefore(API.newDateFixed(this.picker,endRangeDate));
      var isAfter = startMoment.isAfter(API.newDateFixed(this.picker,startRangeDate)) && startMoment.isAfter(API.newDateFixed(this.picker,endRangeDate)) && endMoment.isAfter(API.newDateFixed(this.picker,startRangeDate)) && endMoment.isAfter(API.newDateFixed(this.picker,endRangeDate));
      // 当前显示范围在所选范围外
      if (isAfter || isBefore) {
        return;
      }
      // 开始值在范围内
      if (isStartBetween) {
        index = (startMoment.month() + 1) == startMonth ? 0 : 1;
        $wrap.eq(index).find('td.available').eq(startMoment.date() - 1).addClass('current start-date');
      }
      // 结束值在范围内
      if (isEndBetween) {
        index = (endMoment.month() + 1) == startMonth ? 0 : 1;
        $wrap.eq(index).find('td.available').eq(endMoment.date() - 1).addClass('current end-date');
      }

      var $current = $wrap.find('td.current');
      var $start = $wrap.find('.start-date');
      var $end = $wrap.find('.end-date');
      $current.addClass('in-range');
      // 选中的都在
      // 同一个
      if ($start.is($end)) {
        $start.addClass('in-range');
        return;
      } else if ($current.length === 2) {
        var $startTr = $start.parents('tr');
        var $endTr = $end.parents('tr');
        // 同一页
        if ($start.parents('.c-datepicker-date-range-picker-panel__wrap').is($end.parents('.c-datepicker-date-range-picker-panel__wrap'))) {
          // 同一行
          if ($startTr.is($endTr)) {
            var $tds = $start.nextAll('td.available');
            $tds.each(function (i, _td) {
              $(_td).addClass('in-range');
              if ($(_td).is($end)) {
                return false;
              }
            })
            return;
          }
          $start.nextAll('td.available').addClass('in-range');
          $end.prevAll('td.available').addClass('in-range');
          var $startTrs = $startTr.nextAll('tr');
          var $endTr = $endTr.prev('tr');
          // 上下行，则不用继续
          if ($startTr.is($endTr)) {
            return;
          }
          // 多行
          $startTrs.each(function (i, tr) {
            $(tr).find('td.available').addClass('in-range');
            if ($(tr).is($endTr)) {
              return false;
            }
          })
          return;
        }
        // 不同页
        $start.nextAll('td.available').addClass('in-range');
        $end.prevAll('td.available').addClass('in-range');
        $startTr.nextAll('tr').find('td.available').addClass('in-range');
        $endTr.prevAll('tr').find('td.available').addClass('in-range');
      } else if ($start.length) {
        // 只有开始选中
        var $startTr = $start.parents('tr');
        $start.nextAll('td.available').addClass('in-range');
        $startTr.nextAll('tr').find('td.available').addClass('in-range');
        if (index === 0) {
          $wrap.eq(1).find('td.available').addClass('in-range');
        }
      } else if ($end.length) {
        // 只有结束选中
        var $endTr = $end.parents('tr');
        $end.prevAll('td.available').addClass('in-range');
        $endTr.prevAll('tr').find('td.available').addClass('in-range');
        if (index === 1) {
          $wrap.eq(0).find('td.available').addClass('in-range');
        }
      } else {
        $wrap.find('td.available').addClass('in-range');
      }
    }
  });
  /*==============END DAY============*/

  /*==============BEGIN TIME============*/
  // 时分秒
  function Time(picker) {
    this.picker = picker;
    this.init();
  }

  $.extend(Time.prototype, {
    init: function () {

    },
    event: function () {
      // 时分秒取消
      this.picker.$container.on('click', '.c-datepicker-time-panel__btn.cancel', function () {
        var _this = API.getPicker($(this), 'time');
        var $time = _this.picker.activeTimeWrap.find('.c-datePicker__input-time');
        var index = _this.picker.$container.find('.c-datePicker__input-time').index($time);
        if (!_this.picker.config.isRange) {
          var day = _this.picker.$container.find('.c-datePicker__input-day').eq(index).val();
          _this.picker.$input.val(day + ' ' + _this.prevValue);
        }
        _this.picker.$container.find('.c-datePicker__input-time').eq(index).val(_this.prevValue);
        _this.hide();
      });
      // 时分秒确定
      this.picker.$container.on('click', '.c-datepicker-time-panel__btn.confirm', function () {
        var _this = API.getPicker($(this), 'time');
        _this.hide();
      });
      // 0点
      this.picker.$container.on('click', '.c-datepicker-time-panel__btn.min', function () {
        var _this = API.getPicker($(this), 'time');
        _this.updateTimeInput(_this.picker.timeMin);
      });
      // 23点
      this.picker.$container.on('click', '.c-datepicker-time-panel__btn.max', function () {
        var _this = API.getPicker($(this), 'time');
        _this.updateTimeInput(_this.picker.timeMax);
      });
      // 点击隐藏
      this.picker.$container.on('click', function () {
        var _this = $(this).data('time');
        _this.hide();
      });
      var timerArr={
        timer0:'',
        timer1:'',
        timer2:''
      };
      // 滚动选择时分秒
      this.picker.$container.find('.c-datepicker-scrollbar__wrap').scroll(function () {
        var _this = API.getPicker($(this), 'time');
        var index = _this.picker.$container.find('.c-datepicker-scrollbar__wrap').index($(this));
        // 兼容滚动延时，导致下一个时间滚动清除掉上一个的timer,所以分开是三个
        clearTimeout(timerArr['timer' + index]);
        timerArr['timer' + index] = setTimeout(function () {
          
          var top = $(this).scrollTop();
          var num = Math.round(top / 32);
          var len = $(this).find('li').length - 1;
          if (num >= len) {
            num = len;
          }
          top = num * 32;
          $(this).scrollTop(top);
          var index = _this.picker.activeTimeWrap.find('.c-datepicker-scrollbar__wrap').index($(this));
          var $time = _this.picker.activeTimeWrap.find('.c-datePicker__input-time');
          var day = _this.picker.activeTimeWrap.find('.c-datePicker__input-day').val();
          var val = $time.val();
          val = val.split(':');
          val[index] = API.fillTime(num);
          val = val.join(':');
          $time.val(val);
          if (!_this.picker.config.isRange) {
            // 更新input val
            _this.picker.$input.val(day + ' ' + val);
          }
        }.bind(this), 100);
      })
    },
    updateTimeInput: function (val) {
      this.picker.activeTimeWrap.find('.c-datePicker__input-time').val(val);
      if (!this.picker.config.isRange) {
        var day = this.picker.$input.val().split(' ')[0];
        this.picker.$input.val(day + ' ' + val);
      }
      // this.updateTimePanel();
    },
    updateTimePanel: function (isShow) {
      var $wrap = this.picker.activeTimeWrap.find('.c-datepicker-scrollbar__wrap');
      var val = this.picker.activeTimeWrap.find('.c-datePicker__input-time').val();
      var format = this.picker.config.format.split(' ')[1];
      var regText = format.replace(/HH/, '[0-9]{2}').replace(/(mm|ss)/g, '[0-9]{2}');
      var reg = new RegExp('^' + regText + '$');
      var isMatch = reg.test(val);
      // 判断符合时间格式
      if (isMatch) {
        // 兼容第一次打开才赋值
        if (isShow) {
          this.prevValue = val;
        }
        val = val.split(':');
        $.each($wrap, function (i, el) {
          $(el).scrollTop(Number(val[i]) * 32).addClass('active');
        });
      }
      return isMatch;
    },
    show: function () {
      this.picker.activeTimeWrap.find('.c-datepicker-time-panel').show();
      this.updateTimePanel(true);
    },
    hide: function () {
      this.picker.$container.find('.c-datepicker-time-panel').hide();
    },
    render: function (type, hour, minute, second) {
      if (this.picker.config.isRange) {
        this.renderRange(type, hour, minute, second);
      } else {
        this.renderSingle(type, hour, minute, second);
      }
    },
    renderSingle: function (type, hour, minute, second) {
      var html = this.renderHtml(type, hour, minute, second);
      var $time = this.picker.activeTimeWrap.find('.c-datepicker-time-panel');
      // 初始化，添加html
      if (!$time.length) {
        this.picker.activeTimeWrap.find('.c-datepicker-date-picker__editor-wrap').eq(1).append(html);
        this.picker.$container.data('time', this);
        this.event();
        this.show();
      } else {
        // 已添加，显示
        this.show();
      }
    },
    renderRange: function (type, hour, minute, second) {
      var html = this.renderHtml(type, hour, minute, second);
      var $time = this.picker.activeTimeWrap.find('.c-datepicker-time-panel');
      // 初始化，添加html
      if (!$time.length) {
        var $content = this.picker.$container.find('.c-datepicker-date-range-picker__time-content');
        $content.eq(0).find('.c-datepicker-date-range-picker__editor-wrap').eq(1).append(html);
        $content.eq(1).find('.c-datepicker-date-range-picker__editor-wrap').eq(1).append(html);
        this.picker.$container.find('.c-datepicker-time-panel').hide();
        this.picker.$container.data('time', this);
        this.event();
        this.show();
      } else {
        // 已添加，显示
        this.show();
      }
    },
    renderHtml: function (type, hour, minute, second) {
      hour = hour || moment().hour();
      minute = minute || moment().minute();
      second = second || moment().second();
      var li = '';
      var html = '';
      // 时
      if (type[0]) {
        for (var i = 0; i < 24; i++) {
          var className = hour === i ? 'active' : '';
          li += TIMELITPL.replace('{{time}}', API.fillTime(i)).replace('{{className}}', className);
        }
        html += TIMETPL.replace('{{li}}', li).replace('{{className}}', 'hour');
        li = '';
      }

      // 分
      if (type[1]) {
        for (var j = 0; j < 60; j++) {
          var className = minute === j ? 'active' : '';
          li += TIMELITPL.replace('{{time}}', API.fillTime(j)).replace('{{className}}', className);
        }
        html += TIMETPL.replace('{{li}}', li).replace('{{className}}', 'minute');
        li = '';
      }
      // 秒
      if (type[2]) {
        for (var k = 0; k < 60; k++) {
          var className = second === k ? 'active' : '';
          li += TIMELITPL.replace('{{time}}', API.fillTime(k)).replace('{{className}}', className);
        }
        html += TIMETPL.replace('{{li}}', li).replace('{{className}}', 'second');
      }

      html = TIMEMAINTPL.replace('{{time}}', html);
      return html;
    }
  });

  /*==============END TIME============*/

  /*==============BEGIN PICKER============*/

  // 点击body关闭
  $('body').on('click.datePicker', function () {
    $('.c-datepicker-picker').each(function (i, panel) {
      var _this = $(panel).data('picker');
      if ($(panel).css('display') === 'block') {
        if (_this.config.isRange && (!_this.$inputBegin.val() && !_this.$inputEnd.val())) {
          $(panel).find('td.available').removeClass('current in-range');
        }
        if (_this.hasTime) {
          $(panel).find('.c-datepicker-time-panel').hide();
        }
        _this.config.hide.call(_this);
        _this.datePickerObject.betweenHandle();
      }

    })
    $('.c-datepicker-picker').hide();
  });

  // 父级div.c-datepicker-box滚动，日期选择框跟随input滚动
  $('.c-datepicker-box').scroll(scrollSetContainerPos);
  function scrollSetContainerPos() {
    $('.c-datepicker-picker').each(function (i, panel) {
      var _this = $(panel).data('picker');
      if ($(panel).css('display') === 'block') {
        setContainerPos(_this.datePickerObject);
      }
    })
  }
  
  var DATEPICKERAPI={
    // 初始化年月日十分秒panel
    initShowObject: function(_this, dataFormat){
      var year, month, dayYear, dayMonth, dayDate;
      if (_this.config.isRange) {
        // 默认值回填时间插件选择框
        _this.fillDefault();
        dayYear = [dataFormat[0].year, dataFormat[1].year];
        dayMonth = [dataFormat[0].month, dataFormat[1].month];
        dayDate = [dataFormat[0].day, dataFormat[1].day];
        year = dataFormat[0].year;
        month = dataFormat[0].month;
      } else {
        var inputVal = _this.$input.val();
        year = dataFormat.year;
        month = dataFormat.month;
        dayYear = year;
        dayMonth = month;
        dayDate = inputVal ? dataFormat.day : false;
        // 年
        if (_this.params.format[0]) {
          _this.yearObject = new Year(_this);
          if (!_this.params.format[2] && !_this.params.format[1]) {
            _this.yearObject.render(year);
          }
        }
        // 月
        if (_this.params.format[1]) {
          _this.monthObject = new Month(_this);
          if (!_this.params.format[2]) {
            _this.$container.find('.c-datepicker-date-picker__prev-btn.month,.c-datepicker-date-picker__next-btn.month').hide();
            _this.monthObject.render(month);
          }
        }
      }
      
      // 日
      if (_this.params.format[2]) {
        _this.dayObject = new Day(_this);
        _this.dayObject.render(dayYear, dayMonth, dayDate);
      }
      if (_this.params.format[3] || _this.params.format[4] || _this.params.format[5]) {
        _this.timeObject = new Time(_this);
      }
    },
    // 初始化splitStr，params.format，minJson，maxJson
    initParams: function (_this){
      _this.splitStr = _this.config.format.replace(/[YMDhms:\s]/g, '').split('')[0];
      _this.params.format = API.getFormat(_this.config.format);
      // 最大值最小值判断
      _this.minJson = _this.config.min ? API.getTimeFormat(moment(API.newDateFixed(_this, _this.config.min))) : false;
      _this.maxJson = _this.config.max ? API.getTimeFormat(moment(API.newDateFixed(_this, _this.config.max))) : false;
    },
    renderPicker: function (target, isBlur){
      var _this = API.getPicker($(target));
      if (_this.config.isRange){
        DATEPICKERAPI.renderPickerRange(target, isBlur);
      }else{
        DATEPICKERAPI.renderPickerSingle(target, isBlur);
      }
    }, 
    renderPickerRange: function(target,isBlur){
      var _this = API.getPicker($(target));
      var val = target.value;
      var format = _this.config.format.split(' ')[0];
      var regText = format.replace(/YYYY/, '[0-9]{4}').replace(/(MM|DD)/g, '[0-9]{2}');
      var reg = new RegExp('^' + regText + '$');
      // 判断符合时间格式
      if (reg.test(val)) {
        var $days = _this.$container.find('.c-datePicker__input-day');
        var $times = _this.$container.find('.c-datePicker__input-time');
        var index = $days.index($(target));
        var isBaseEnd = index === 1;
        var anotherVal = $days.eq(1 - index).val();
        var _moment = moment(API.newDateFixed(_this, val));
        var _momentAnother = moment(API.newDateFixed(_this, anotherVal));
        var orderFail = index === 0 ? _moment.isAfter(_momentAnother) : _moment.isBefore(_momentAnother);
        // 反过来，需要交换
        if (orderFail) {
          var temp = val;
          val = anotherVal;
          anotherVal = temp;
          _moment = moment(API.newDateFixed(_this, val));
          _momentAnother = moment(API.newDateFixed(_this, anotherVal));
          $days.eq(index).val(val);
          $days.eq(1 - index).val(anotherVal);
        }
        // 十分秒重置

        if (_this.hasTime && !isBlur) {
          $times.eq(0).val(_this.timeMin);
          $times.eq(1).val(_this.timeMax);
        }
        // if (_this.dayObject.isBlur) {
        //   _this.dayObject.isBlur = false;
        // }

        var resultAnother = API.getTimeFormat(_momentAnother);
        var result = API.getTimeFormat(_moment);
        // var month = result.month-1;
        // 最大最小值判断修改
        var resultJson = API.minMaxFill(_this, result, index);
        result = resultJson.result;
        // 填充值
        target.value = resultJson.val;
        // this.value = result.year + _this.splitStr + API.fillTime(month + 1) + _this.splitStr + API.fillTime(result.day);
        var rangeYears = [], rangeMonths = [], rangeDates = [];
        rangeYears[index] = result.year;
        rangeMonths[index] = result.month;
        rangeDates[index] = result.day;
        rangeYears[1 - index] = resultAnother.year;
        rangeMonths[1 - index] = resultAnother.month;
        rangeDates[1 - index] = resultAnother.day;
        _this.dayObject.renderRange(rangeYears, rangeMonths, rangeDates, isBaseEnd, true);
      }
    },
    renderPickerSingle: function (target){
      var _this = API.getPicker($(target));
      var val = target.value;
      var format = _this.config.format.split(' ')[0];
      var regText = format.replace(/YYYY/, '[0-9]{4}').replace(/(MM|DD)/g, '[0-9]{2}');
      var reg = new RegExp('^' + regText + '$');
      // 判断符合时间格式
      if (reg.test(val)) {
        var $time = _this.$container.find('.c-datePicker__input-time');
        var _moment = moment(API.newDateFixed(_this, val));
        var result = API.getTimeFormat(_moment);
        // 最大最小值判断修改
        var resultJson = API.minMaxFill(_this, result, 0);
        result = resultJson.result;
        val = resultJson.val;
        // 填充值
        target.value = val;
        if (_this.hasTime) {
          val += ' ' + $time.val();
        }
        _this.$input.val(val);
        _this.dayObject.renderSingle(result.year, result.month, result.day, true);
      }
    },
    cancelBlur: function (_this) {
      $.unsub('datapickerRenderPicker');
      _this.isBlur = false;
    }
  }

  function SingleDatePicker(datePickerObject) {
    this.datePickerObject = datePickerObject;
    this.datePickerObject.pickerObject=null;
    this.$input = datePickerObject.$target.find('input');
    this.config = datePickerObject.config;
    this.params = {};
    this.hasTime = this.config.format.split(' ').length > 1;
    if (this.hasTime) {
      this.timeMin = API.timeVal(this, 'min');
      this.timeMax = API.timeVal(this, 'max');
    }
    this.init();
  }

  $.extend(SingleDatePicker.prototype, {
    init: function () {
      this.initShow();
      this.event();
    },

    initShow: function () {
      // 初始化splitStr，params.format，minJson，maxJson
      DATEPICKERAPI.initParams(this);
      this.params.isYear = this.params.format[0] && !this.params.format[1];
      this.params.isMonth = this.params.format[0] && this.params.format[1] && !this.params.format[2];
      var table = '';
      var inputVal = this.$input.val();
      var result = inputVal ? moment(API.newDateFixed(this,inputVal)) : moment();
      var dataFormat = API.getTimeFormat(result);
      var sidebar = '';
      var hasSidebar = '';
      var hasTime = '';
      if (this.params.format[3] || this.params.format[4] || this.params.format[5]) {
        hasTime = 'has-time';
      }
      // 有快捷键-单个还是范围
      if (this.config.hasShortcut) {
        hasSidebar = 'has-sidebar';
        sidebar = rederSidebar(this);
      }
      var renderTpl = DATEPICKERMAINTPL;
      if (this.params.isYear || this.params.isMonth) {
        renderTpl = renderTpl.replace(/{{footerButton}}/g, PICKERFOOTERCLEARBUTTON);
      } else {
        renderTpl = renderTpl.replace(/{{footerButton}}/g, PICKERFOOTERNOWBUTTON);
      }

      var $datePickerHtml = $(renderTpl.replace(/{{table}}/g, table).replace(/{{year}}/g, dataFormat.year).replace(/{{month}}/g, dataFormat.month).replace('{{sidebar}}', sidebar).replace('{{hasTime}}', hasTime).replace('{{hasSidebar}}', hasSidebar));
      $('body').append($datePickerHtml);
      this.$container = $datePickerHtml;
      this.$container.data('picker', this);
      // 没有十分秒
      if (!this.hasTime) {
        this.$container.find('.c-datepicker-date-picker__time-header').hide();
      }
      // 初始化年月日十分秒panel
      DATEPICKERAPI.initShowObject(this, dataFormat);
      // 默认值回填时间插件选择框
      var val = this.$input.val().split(' ');
      this.$container.find('.c-datePicker__input-day').val(val[0]);
      this.$container.find('.c-datePicker__input-time').val(val[1]);


    },
    event: function () {
      if (this.hasTime) {
        this.eventHasTime();
      }
      this.datePickerObject.$target.on('click', function (event) {
        event.stopPropagation();
      });
      this.$container.on('click', function (event) {
        event.stopPropagation();
      });

      // 点击选择年
      this.$container.on('click', '.c-datepicker-date-picker__header-year', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        if (_this.isBlur){
          DATEPICKERAPI.cancelBlur(_this);
        }
        if ($(this).hasClass('disabled')) {
          return;
        }
        
        var val = _this.$input.val();
        if (!val) {
          val = moment();
        } else {
          val = moment(API.newDateFixed(_this,val));
        }
        _this.yearObject.render(val.year());
      })
      // 点击选择月
      this.$container.on('click', '.c-datepicker-date-picker__header-month', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        if (_this.isBlur) {
          DATEPICKERAPI.cancelBlur(_this);
        }
        if ($(this).hasClass('disabled')) {
          return;
        }
        var val = _this.$input.val();
        if (!val) {
          val = moment();
        } else {
          val = moment(API.newDateFixed(_this,val));
        }
        _this.monthObject.render(val.month() + 1);
      })
      // 下一月
      this.$container.on('click', '.c-datepicker-date-picker__next-btn.month', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        renderYearMonth(_this,'next', 'month');
      })
      // 上一月
      this.$container.on('click', '.c-datepicker-date-picker__prev-btn.month', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        // 操作选择年的翻页
        renderYearMonth(_this,'prev', 'month');
      })
      // 下一年
      this.$container.on('click', '.c-datepicker-date-picker__next-btn.year', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        // 操作选择年的翻页
        if ($(this).hasClass('is-year')) {
          var newYear = Number(_this.$container.find('.c-datepicker-year-table td.available').first().find('.cell').text()) + 10;
          _this.yearObject.render(newYear);
        } else if ($(this).hasClass('is-month')) {
          var $year = _this.$container.find('.c-datepicker-date-picker__header-year span');
          $year.text(Number($year.text()) + 1);
          _this.monthObject.render();
        } else {
          renderYearMonth(_this,'next', 'year');
        }

      })
      // 上一年
      this.$container.on('click', '.c-datepicker-date-picker__prev-btn.year', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        // 操作选择年的翻页
        if ($(this).hasClass('is-year')) {
          var newYear = Number(_this.$container.find('.c-datepicker-year-table td.available').first().find('.cell').text()) - 10;
          _this.yearObject.render(newYear);
        } else if ($(this).hasClass('is-month')) {
          var $year = _this.$container.find('.c-datepicker-date-picker__header-year span');
          $year.text(Number($year.text()) - 1);
          _this.monthObject.render();
        } else {
          renderYearMonth(_this,'prev', 'year');
        }
      })
      // 兼容blur点击切换年时，blur需要重新渲染data，
      function renderYearMonth(_this, dire, type) {
        if (_this.isBlur) {
          _this.dayObject.prevNextSingle(dire, type);
          $.unsub('datapickerRenderPicker');
          _this.isBlur=false;
          // $.sub('datapickerClick', function (e) {
          //   _this.dayObject.prevNextSingle(dire, type);
          //   $.unsub('datapickerClick');
          // });
        } else {
          _this.dayObject.prevNextSingle(dire, type);
        }
      }
      // 点击此刻
      this.$container.on('click', '.c-datepicker-picker__btn-now', function () {
        var _this = API.getPicker($(this));
        setValue(_this, moment().format(_this.config.format));
        _this.datePickerObject.hide();
      });

      // 点击清空
      this.$container.on('click', '.c-datepicker-picker__btn-clear', function () {
        var _this = API.getPicker($(this));
        _this.clear();
      })

      // 点击快捷选项
      this.$container.on('click', '.c-datepicker-picker__shortcut', function () {
        var _this = API.getPicker($(this));
        var day = $(this).data('value');
        var result = moment().add(day, 'day').format(_this.config.format);
        if (_this.hasTime) {
          var time = $(this).data('time');
          if (time) {
            result = result.split(' ')[0] + ' ' + time;
          }
        }
        setValue(_this, result);
        _this.datePickerObject.hide();
      });

      // 点击确定
      this.$container.on('click', '.c-datepicker-picker__link-btn.confirm', function () {
        var _this = API.getPicker($(this));
        if (!_this.$input.val()) {
          setValue(_this, moment().format(_this.config.format));
        }
        _this.datePickerObject.hide();
      });
    },
    eventHasTime: function () {
      // 输入框修改日期input
      this.$container.on('keyup', '.c-datePicker__input-time', function () {
        var _this = API.getPicker($(this));
        // 更新显示的time panel值
        var isMatch = _this.timeObject.updateTimePanel();
        if (isMatch) {
          var time = this.value;
          var day = _this.$container.find('.c-datePicker__input-day').val();
          _this.$input.val(day + ' ' + time);
        }

      })
      this.$container.on('click', '.c-datePicker__input-time', function (event) {
        event.stopPropagation();
      });
      // 修改日期
      this.$container.on('keyup', '.c-datePicker__input-day', function () {
        DATEPICKERAPI.renderPickerSingle(this);
      });
      // 失焦判断最大值最小值
      this.$container.on('blur', '.c-datePicker__input-day', function (event) {
        var _this = API.getPicker($(this));
        // 修复满足格式但不完全符合的day格式修正
        fillDay(_this, $(this));
        API.judgeTimeRange(_this, $(this), _this.$container.find('.c-datePicker__input-time'));
      });
      // 失焦判断最大值最小值
      this.$container.on('blur', '.c-datePicker__input-time', function (event) {
        var _this = API.getPicker($(this));
        // 修复满足格式但不完全符合的time格式修正
        fillTime(_this, $(this));
        API.judgeTimeRange(_this, _this.$container.find('.c-datePicker__input-day'), $(this));
      });

      // 聚焦时分秒input
      this.$container.on('focus', '.c-datePicker__input-time', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        if (!_this.$input.val() && !this.value) {
          var now = moment().format(_this.config.format);
          _this.$input.val(now);
          now = now.split(' ');
          _this.$container.find('.c-datePicker__input-day').val(now[0]);
          $(this).val(now[1]);
        }
        _this.activeTimeWrap = $(this).parents('.c-datepicker-date-picker__time-header');
        var val = this.value.split(':');
        _this.showTimeSelect(val[0], val[1], val[2]);
      });
      // 聚焦日期input
      this.$container.on('focus', '.c-datePicker__input-day', function () {
        var _this = API.getPicker($(this));
        if (!_this.$input.val()) {
          var now = moment().format(_this.config.format).split(' ');
          $(this).val(now[0]);
          if (now.length > 1) {
            _this.$container.find('.c-datePicker__input-time').val(now[1]);
          }
        }
      });
    },
    clear: function () {
      this.$input.val('');
      this.$container.find('td.available').removeClass('current');
    },
    show: function () {
      // 日
      if (this.params.format[2]) {
        var val = API.getRangeTimeFormat(this,this.$input);
        this.dayObject.render(val.year, val.month, val.day, true);
      }
      this.$container.show();
    },
    reRenderDay: function () {
      // 日
      if (this.params.format[2]) {
        var result = API.getRangeTimeFormat(this,this.$input);
        // 判断是否选中
        var _val = this.$input.val() ? result.day : false;
        this.dayObject.render(result.year, result.month, _val, true);
      }
    },
    renderYear: function () {
      this.yearObject.render();
    },
    renderMonth: function () {
      this.monthObject.render();
    },
    showTimeSelect: function (year, month, day) {
      if (this.params.format[3] || this.params.format[4] || this.params.format[5]) {
        this.timeObject.render(this.params.format.slice(3), year, month, day);
      }
    }
    
  });

  // 时间范围
  function RangeDatePicker(datePickerObject) {
    this.datePickerObject = datePickerObject;
    this.datePickerObject.pickerObject = null;
    this.$input = datePickerObject.$target.find('input');
    this.$inputBegin = this.$input.eq(0);
    this.$inputEnd = this.$input.eq(1);
    this.config = datePickerObject.config;
    this.params = {};
    this.hasTime = this.config.format.split(' ').length > 1;
    if (this.hasTime) {
      this.timeMin = API.timeVal(this, 'min');
      this.timeMax = API.timeVal(this, 'max');
    }
    this.init();
  }

  $.extend(RangeDatePicker.prototype, {
    init: function () {
      this.initShow();
      this.event();
    },

    initShow: function () {
      // 初始化splitStr，params.format，minJson，maxJson
      DATEPICKERAPI.initParams(this);
      var table = '';
      var dataFormat = [];
      dataFormat[0] = API.getRangeTimeFormat(this,this.$input.eq(0));
      dataFormat[1] = API.getRangeTimeFormat(this,this.$input.eq(1));
      var sidebar = '';
      var hasSidebar = '';
      var hasTime = '';
      if (this.params.format[3] || this.params.format[4] || this.params.format[5]) {
        hasTime = 'has-time';
      }
      // 有快捷键-单个还是范围
      if (this.config.hasShortcut) {
        hasSidebar = 'has-sidebar';
        sidebar = rederSidebar(this);
      }
      var $datePickerHtml = $(RANGEPICKERMAINTPL.replace(/{{table}}/g, table).replace(/{{year}}/g, dataFormat[0].year).replace(/{{month}}/g, dataFormat[0].month).replace(/{{yearEnd}}/g, dataFormat[1].year).replace(/{{monthEnd}}/g, dataFormat[1].month).replace('{{sidebar}}', sidebar).replace('{{hasTime}}', hasTime).replace('{{hasSidebar}}', hasSidebar));
      $('body').append($datePickerHtml);
      this.$container = $datePickerHtml;
      this.$container.data('picker', this);
      // 没有十分秒
      if (!this.hasTime) {
        this.$container.find('.c-datepicker-date-range-picker__time-header').hide();
      }
      // 初始化年月日十分秒panel
      DATEPICKERAPI.initShowObject(this, dataFormat);
    },
    // 默认值回填时间插件选择框
    fillDefault: function () {
      var valBegin = this.$inputBegin.val().split(' ');
      var valEnd = this.$inputEnd.val().split(' ');
      var $day = this.$container.find('.c-datePicker__input-day');
      var $time = this.$container.find('.c-datePicker__input-time');
      if (valBegin) {
        $day.eq(0).val(valBegin[0]);
        $time.eq(0).val(valBegin[1]);
      }
      if (valEnd) {
        $day.eq(1).val(valEnd[0]);
        $time.eq(1).val(valEnd[1]);
      }
    },
    event: function () {
      if (this.hasTime) {
        this.eventHasTime();
      }
      this.$container.on('click', function (event) {
        event.stopPropagation();
      });
      this.datePickerObject.$target.on('click', function (event) {
        event.stopPropagation();
      });

      // 下一月
      this.$container.on('click', '.c-datepicker-date-range-picker__next-btn.month', function () {
        var _this = API.getPicker($(this));
        renderYearMonth(_this, 'next', 'month');
      })
      // 上一月
      this.$container.on('click', '.c-datepicker-date-range-picker__prev-btn.month', function () {
        var _this = API.getPicker($(this));
        renderYearMonth(_this,'prev', 'month');
      })
      // 下一年
      this.$container.on('click', '.c-datepicker-date-range-picker__next-btn.year', function () {
        var _this = API.getPicker($(this));
        renderYearMonth(_this,'next', 'year');
      })
      // 上一年
      this.$container.on('click', '.c-datepicker-date-range-picker__prev-btn.year', function () {
        var _this = API.getPicker($(this));
        renderYearMonth(_this,'prev', 'year');
      })

      function renderYearMonth(_this,dire,type){
        if (_this.isBlur) {
          $.sub('datapickerClick', function (e) {
            _this.dayObject.prevNextRender(dire,type);
            $.unsub('datapickerClick');
          });
          $.pub('datapickerRenderPicker');
        } else {
          _this.dayObject.prevNextRender(dire,type);
        }
      }
      // 点击清空
      this.$container.on('click', '.c-datepicker-picker__btn-clear', function () {
        var _this = API.getPicker($(this));
        _this.clear();
      })

      // 快捷选项
      this.$container.on('click', '.c-datepicker-picker__shortcut', function () {
        var _this = API.getPicker($(this));
        var days = $(this).data('value').split(',');
        var begin = moment().add(days[0], 'day').format(_this.config.format);
        var end = moment().add(days[1], 'day').format(_this.config.format);
        if (_this.hasTime) {
          var times = $(this).data('time').split(',');
          if (times[0]) {
            begin = begin.split(' ')[0] + ' ' + times[0];
          }
          if (times[1]) {
            end = end.split(' ')[0] + ' ' + times[1];
          }
        }
        _this.$inputBegin.val(begin);
        _this.$inputEnd.val(end);
        _this.datePickerObject.hide();
        // setValue(_this, result);
      });

      // 点击确定
      this.$container.on('click', '.c-datepicker-picker__link-btn.confirm', function () {
        var _this = API.getPicker($(this));
        var $days = _this.$container.find('.c-datePicker__input-day');
        var $times = _this.$container.find('.c-datePicker__input-time');
        var start = $days.eq(0).val();
        var end = $days.eq(1).val();
        if (!start||!end){
          _this.datePickerObject.hide();
          return;
        }
        if (_this.hasTime) {
          start += ' ' + $times.eq(0).val();
          end += ' ' + $times.eq(1).val();
        }
        _this.$inputBegin.val(start);
        _this.$inputEnd.val(end);
        _this.datePickerObject.hide();
      });
    },
    eventHasTime: function () {
      // 输入框修改日期input
      this.$container.on('keyup', '.c-datePicker__input-time', function () {
        var _this = API.getPicker($(this));
        _this.timeObject.updateTimePanel();
      })
      // 输入框修改日期input
      this.$container.on('keyup', '.c-datePicker__input-day', function () {
        DATEPICKERAPI.renderPicker(this);
      });
      
      this.$container.on('click', '.c-datePicker__input-time', function (event) {
        event.stopPropagation();
      });
      // 聚焦时分秒input
      this.$container.on('focus', '.c-datePicker__input-time', function (event) {
        event.stopPropagation();
        var _this = API.getPicker($(this));
        if (!_this.$input.val() && !this.value) {
          var now = moment().format(_this.config.format);
          // _this.$input.val(now);
          now = now.split(' ');
          _this.$container.find('.c-datePicker__input-day').val(now[0]);
          _this.$container.find('.c-datePicker__input-time').val(now[1]);
        }
        _this.activeTimeWrap = $(this).parents('.c-datepicker-date-range-picker__time-content');
        _this.showTimeSelect();
        $(this).trigger('keyup');
      });
      // 聚焦日期input
      this.$container.on('focus', '.c-datePicker__input-day,.c-datePicker__input-time', function () {
        var _this = API.getPicker($(this));
        var $day = _this.$container.find('.c-datePicker__input-day');
        if (!$(this).val()) {
          var now = moment().format(_this.config.format).split(' ');
          $day.val(now[0]);
          if (now.length > 1) {
            _this.$container.find('.c-datePicker__input-time').val(now[1]);
          }
        }
      });
      // day失焦判断最大值最小值
      this.$container.on('blur', '.c-datePicker__input-day', function (event) {
        var _this = API.getPicker($(this));
        var index = _this.$container.find('.c-datePicker__input-day').index($(this));
        // 修复满足格式但不完全符合的day格式修正
        fillDay(_this, $(this));
        API.judgeTimeRange(_this, $(this), _this.$container.find('.c-datePicker__input-time').eq(index), index);
      });
      // time失焦判断最大值最小值
      this.$container.on('blur', '.c-datePicker__input-time', function (event) {
        var _this = API.getPicker($(this));
        var index = _this.$container.find('.c-datePicker__input-time').index($(this));
        // 修复满足格式但不完全符合的time格式修正（先修正，后比较大小）
        fillTime(_this, $(this));
        API.judgeTimeRange(_this, _this.$container.find('.c-datePicker__input-day').eq(index), $(this), index);
      });
    },
    show: function () {
      this.fillDefault();
      var dataFormat = [];
      dataFormat[0] = API.getRangeTimeFormat(this,this.$input.eq(0));
      dataFormat[1] = API.getRangeTimeFormat(this,this.$input.eq(1));
      var yearArr = [dataFormat[0].year, dataFormat[1].year];
      var monthArr = [dataFormat[0].month, dataFormat[1].month];
      var dayArr = [dataFormat[0].day, dataFormat[1].day];
      // 日
      if (this.params.format[2]) {
        this.dayObject.render(yearArr, monthArr, dayArr, false, true);
      }
      this.$container.show();
    },
    clear: function () {
      this.$inputBegin.val('');
      this.$inputEnd.val('');
      this.$container.find('.c-datePicker__input-day,.c-datePicker__input-time').val('');
      this.$container.find('td.available').removeClass('current in-range');
    },
    renderYear: function () {
      this.yearObject.render();
    },
    renderMonth: function () {
      this.monthObject.render();
    },
    showTimeSelect: function () {
      if (this.params.format[3] || this.params.format[4] || this.params.format[5]) {
        this.timeObject.render(this.params.format.slice(3));
      }
    }
  });

  function DatePicker(options, ele) {
    // this.$container = $('.c-datepicker-picker');
    this.$target = ele;
    this.config = $.extend({}, defaultOptions, options);
    this.params = {};
    this.init();
  }

  $.extend(DatePicker.prototype, {
    init: function () {
      if (!this.config.isRange) {
        this.pickerObject = new SingleDatePicker(this);
      } else {
        this.pickerObject = new RangeDatePicker(this);
      }
      this.pickerObject.$input.data('datepicker', this);
      this.event();
    },
    event: function () {
      this.pickerObject.$input.on('click', function () {
        var _this = $(this).data('datepicker');
        _this.show();
      });

      this.pickerObject.$input.on('focus', function () {
        var _this = $(this).data('datepicker');
        _this.initInputVal = this.value;
      });
      // 兼容输入框失去焦点
      this.pickerObject.$container.on('click',function(){
        var _this=$(this).data('picker');
        if (_this.isBlur){
          $.unsub('datapickerClick');
          $.pub('datapickerRenderPicker');
          _this.isBlur=false;
        }
      })
      // 输入框失去焦点
      this.pickerObject.$input.on('blur', function () {
        if (!this.value) {
          return;
        }
        var _this = $(this).data('datepicker');
        var index = _this.pickerObject.$input.index($(this));
        var valArr = this.value.split(' ');
        var day = valArr[0];
        var $container = _this.pickerObject.$container;
        // 有十分秒

        if (_this.pickerObject.hasTime) {
          var dayReg = API.dayReg(_this.pickerObject);
          var time = valArr[1] ? API.timeCheck(valArr[1]) : false;
          var $time = $container.find('.c-datePicker__input-time');
          var $day = $container.find('.c-datePicker__input-day');
          var timeResult = time && time.match(API.timeReg(_this));
          var dayResult = day.match(dayReg);
          if (!time || !timeResult || !dayResult) {
            day = _this.initInputVal.split(' ')[0];
            time = _this.initInputVal.split(' ')[1];
            this.value = _this.initInputVal;
          } else {
            if (dayResult) {
              // 兼容201808变为2018-00-08的情况
              dayResult = API.fixedFill(dayResult);
              day = dayResult[1] + _this.pickerObject.splitStr + API.fillTime(dayResult[3]) + _this.pickerObject.splitStr + API.fillTime(dayResult[5])
            }
            if (timeResult) {
              time = timeResult[5] ? timeResult[1] + ':' + API.fillTime(timeResult[3]) + ':' + API.fillTime(timeResult[5]) : timeResult[1] + ':' + API.fillTime(timeResult[3]);
            }
            this.value = day + ' ' + time;
          }
          $time.eq(index).val(time);
          $day.eq(index).val(day);
          // 兼容失去焦点，点击选择日期
          _this.pickerObject.isBlur = true;
          // 逻辑：先blur-绑定重新渲染date panel事件-触发选择日期或年月切换等事件-发布重新渲染事件|取消发布事件
          // 处理的类型有，点击切换上下一年月，点击年月选择，选择日期，点击piker其他
          $.sub('datapickerRenderPicker',function(){
            DATEPICKERAPI.renderPicker($day.eq(index)[0], true);
            _this.pickerObject.isBlur = false;
            $.pub('datapickerClick');
            $.unsub('datapickerRenderPicker');
          });
        } else {
          // 没有十分秒
          // 年月格式
          if (_this.pickerObject.params.isMonth) {
            var _moment = moment(API.newDateFixed(_this.pickerObject, day + _this.pickerObject.splitStr+'01'));
            var result = API.getTimeFormat(_moment);
            var resultJson = API.minMaxFill(_this.pickerObject, result, 0, 'month');
            val = resultJson.val;
            $(this).val(val);
            // _this.hide();
          } else if (_this.pickerObject.params.isYear) {
            if (_this.config.min && day < _this.config.min) {
              day = _this.config.min;
            }
            if (_this.config.max && day > _this.config.max) {
              day = _this.config.max;
            }
            $(this).val(day);
          } else {
            var dayReg = API.dayReg(_this.pickerObject);
            var dayResult = day.match(dayReg);
            if (!dayResult) {
              // 不匹配
              this.value = _this.initInputVal;
            } else {
              // 匹配
              // 兼容201808变为2018-00-08的情况
              dayResult = API.fixedFill(dayResult);
              day = dayResult[1] + _this.pickerObject.splitStr + API.fillTime(dayResult[3]) + _this.pickerObject.splitStr + API.fillTime(dayResult[5]);
              this.value = day;
            }
          }
        }

      });
    },
    show: function () {
      setContainerPos(this);
      $('.c-datepicker-picker').hide();
      this.pickerObject.show();
      this.config.show.call(this.pickerObject);
    },
    hide: function () {
      // 判断输入框没有值
      this.pickerObject.$container.find('.td.available').removeClass('current in-range');
      this.pickerObject.$container.find('.c-datepicker-time-panel').hide();
      this.pickerObject.$container.hide();
      this.betweenHandle();
      this.config.hide.call(this.pickerObject);
    },
    betweenHandle: function () {
      var _config = this.config;
      // 处理范围间距，检测开始结束间隔时间
      if (!_config.isRange || !_config.between) {
        return false;
      }
      var start = this.pickerObject.$inputBegin.val();
      var end = this.pickerObject.$inputEnd.val();
      if (!start || !end) {
        return false;
      }
      var beginMoment = moment(API.newDateFixed(this.pickerObject,start));
      var endMoment = moment(API.newDateFixed(this.pickerObject,end));
      var beginFormat = API.getTimeFormat(beginMoment);
      var endFormat = API.getTimeFormat(endMoment);
      // 同一个月内
      if (_config.between === 'month') {
        if (beginFormat.year !== endFormat.year || beginFormat.month !== endFormat.month) {
          var val = beginMoment.set({ 'year': endFormat.year, 'month': endFormat.month - 1, 'date': 1 }).format(_config.format);
          this.pickerObject.$inputBegin.val(val);
        }
        return;
      }
      // 同一年内
      if (_config.between === 'year') {
        if (beginFormat.year !== endFormat.year) {
          var val = beginMoment.set({ 'year': endFormat.year, 'month': 0, 'date': 1 }).format(_config.format);
          this.pickerObject.$inputBegin.val(val);
        }
        return;
      }
      // 规定天数内
      if (Number.isInteger(Number(_config.between))) {
        var endRangeMoment = endMoment.add(-Number(_config.between), 'day');
        if (endRangeMoment.isAfter(beginMoment)) {
          var val = endRangeMoment.format(_config.format);
          this.pickerObject.$inputBegin.val(val);
        }
      }
    }
  });
  // 设置日期选择框位置
  function setContainerPos(_this) {
    var offset = _this.$target.offset();
    var height = _this.$target.outerHeight();
    _this.pickerObject.$container.css({
      top: offset.top + height,
      left: offset.left
    });
  }
  function fillTime(_this, $time) {
    // 修复满足格式但不完全符合的time格式修正
    var time = $time.val();
    var timeResult = time && time.match(API.timeReg(_this));
    if (!time || !timeResult) {
      return;
    } else {
      if (timeResult) {
        time = _this.config.format.split(' ')[1].replace(/HH/, timeResult[1]).replace(/mm/, API.fillTime(timeResult[3])).replace(/ss/, API.fillTime(timeResult[5]))
        // timeResult[1] + ':' + API.fillTime(timeResult[3]) + ':' + API.fillTime(timeResult[5]);
        $time.val(time);
        if (!_this.config.isRange) {
          $time.trigger('keyup');
        }
      }
    }
  }
  function fillDay(_this, $day) {
    // 修复满足格式但不完全符合的day格式修正
    var day = $day.val();
    var reg = API.dayReg(_this);
    var dayResult = day.match(reg);
    if (!day || !dayResult) {
      return;
    } else {
      if (dayResult) {
        // 兼容201808变为2018-00-08的情况
        dayResult = API.fixedFill(dayResult);
        day = dayResult[1] + _this.splitStr + API.fillTime(dayResult[3]) + _this.splitStr + API.fillTime(dayResult[5]);
        $day.val(day);
        if (!_this.config.isRange) {
          $day.trigger('keyup');
        }
      }
    }
  }

  // renderSidebar
  function rederSidebar(_this) {
    var html = '';
    var options = _this.config.shortcutOptions;
    for (var i = 0; i < options.length; i++) {
      var time = options[i].time || '';
      html += SIDEBARBUTTON.replace('{{name}}', options[i].name).replace('{{day}}', options[i].day).replace('{{time}}', time);
    }
    return SIDEBARTPL.replace('{{button}}', html);
  }

  // 设置选中值
  function setValue(_this, date) {
    _this.$container.find('.c-datepicker-date-table td.current').removeClass('current');
    // var date = _moment2.format(_this.config.format);
    var timeArr = date.split(' ');
    _this.$input.val(date);
    _this.$container.find('.c-datePicker__input-day').val(timeArr[0]);
    if (timeArr.length > 1) {
      _this.$container.find('.c-datePicker__input-time').val(timeArr[1]);
    }
  }

  /*========END 渲染表格===========*/
  $.fn.datePicker = function (options) {
    return this.each(function () {
      new DatePicker(options, $(this));
    });
  };
  /*==============END PICKER============*/
})

// var DATAPICKERAPI = {
//   // 快捷选项option:当前月
//   activeMonthRange: function () {
//     return {
//       begin: moment().set({ 'date': 1, 'hour': 0, 'minute': 0, 'second': 0 }).format('YYYY-MM-DD HH:mm:ss'),
//       end: moment().set({ 'hour': 23, 'minute': 59, 'second': 59 }).format('YYYY-MM-DD HH:mm:ss')
//     }
//   },
//   shortcutMonth: function () {
//     // 当月
//     var nowDay = moment().get('date');
//     var prevMonthFirstDay = moment().subtract(1, 'months').set({ 'date': 1 });
//     var prevMonthDay = moment().diff(prevMonthFirstDay, 'days');
//     return {
//       now: '-' + nowDay + ',0',
//       prev: '-' + prevMonthDay + ',-' + nowDay
//     }
//   },
//   // 快捷选项option:只能同一个月份内的
//   rangeMonthShortcutOption1: function () {
//     var result = DATAPICKERAPI.shortcutMonth();
//     return [{
//       name: '昨天',
//       day: '-1,-1',
//       time: '00:00:00,23:59:59'
//     }, {
//       name: '当月',
//       day: result.now,
//       time: '00:00:00,'
//     }, {
//       name: '上一月',
//       day: result.prev,
//       time: '00:00:00,23:59:59'
//     }];
//   },
//   // 快捷选项option
//   rangeShortcutOption1: [{
//     name: '最近一周',
//     day: '-7,0'
//   }, {
//     name: '最近一个月',
//     day: '-30,0'
//   }, {
//     name: '最近三个月',
//     day: '-90, 0'
//   }],
//   singleShortcutOptions1: [{
//     name: '今天',
//     day: '0'
//   }, {
//     name: '昨天',
//     day: '-1',
//     time: '00:00:00'
//   }, {
//     name: '一周前',
//     day: '-7'
//   }]
// };