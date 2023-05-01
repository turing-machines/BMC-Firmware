/*
Template Name: Greeva - Responsive Bootstrap 4 Admin Dashboard
Author: CoderThemes
File: Form Advanced
*/


!function($) {
    'use strict';

    var AdvanceFormApp = function() {
        this.$body = $('body'),
        this.$window = $(window)
    };


    /** 
     * Initlizes the select2
    */
    AdvanceFormApp.prototype.initSelect2 = function() {
        // Select2
        $(".select2").select2();

        // Select2 with limiting
        $(".select2-limiting").select2({
            maximumSelectionLength: 2
        });
    },

    AdvanceFormApp.prototype.initMaxLength = function() {
        //Bootstrap-MaxLength
        $('input#defaultconfig').maxlength({
            warningClass: "badge badge-success",
            limitReachedClass: "badge badge-danger"
        });

        $('input#thresholdconfig').maxlength({
            threshold: 20,
            warningClass: "badge badge-success",
            limitReachedClass: "badge badge-danger"
        });

        $('input#alloptions').maxlength({
            alwaysShow: true,
            separator: ' out of ',
            preText: 'You typed ',
            postText: ' chars available.',
            validate: true,
            warningClass: "badge badge-success",
            limitReachedClass: "badge badge-danger"
        });

        $('textarea#textarea').maxlength({
            alwaysShow: true,
            warningClass: "badge badge-success",
            limitReachedClass: "badge badge-danger"
        });

        $('input#placement').maxlength({
            alwaysShow: true,
            placement: 'top-left',
            warningClass: "badge badge-success",
            limitReachedClass: "badge badge-danger"
        });
    },

    AdvanceFormApp.prototype.initTimePicker = function() {
        // Time Picker
        jQuery('#timepicker').timepicker({
            defaultTIme: false,
            icons: {
                up: 'mdi mdi-chevron-up',
                down: 'mdi mdi-chevron-down'
            }
        });
        jQuery('#timepicker2').timepicker({
            showMeridian: false,
            icons: {
                up: 'mdi mdi-chevron-up',
                down: 'mdi mdi-chevron-down'
            }
        });
        jQuery('#timepicker3').timepicker({
            minuteStep: 10,
            icons: {
                up: 'mdi mdi-chevron-up',
                down: 'mdi mdi-chevron-down'
            }
        });
    },
    AdvanceFormApp.prototype.initColorPicker = function() {
        //colorpicker start

        $('.colorpicker-default').colorpicker({
            format: 'hex'
        });
        $('.colorpicker-rgba').colorpicker();
    },
    /** 
     * Initlized mask
    */
    AdvanceFormApp.prototype.initMask = function() {
        $('.date').mask('00/00/0000');
        $('.time').mask('00:00:00');
        $('.date_time').mask('00/00/0000 00:00:00');
        $('.cep').mask('00000-000');
        $('.crazy_cep').mask('0-00-00-00');
        $('.phone').mask('0000-0000');
        $('.phone_with_ddd').mask('(00) 0000-0000');
        $('.phone_us').mask('(000) 000-0000');
        $('.mixed').mask('AAA 000-S0S');
        $('.cpf').mask('000.000.000-00', { reverse: true });
        $('.cnpj').mask('00.000.000/0000-00', { reverse: true });
        $('.money').mask('000.000.000.000.000,00', { reverse: true });
        $('.money2').mask("#.##0,00", { reverse: true });
        $('.ip_address').mask('099.099.099.099');
        $('.percent').mask('##0,00%', { reverse: true });
    },

    AdvanceFormApp.prototype.initDatePicker = function() {
        // Date Picker
        jQuery('#datepicker').datepicker();
        jQuery('#datepicker-autoclose').datepicker({
            autoclose: true,
            todayHighlight: true
        });
        jQuery('#datepicker-inline').datepicker();
        jQuery('#datepicker-multiple-date').datepicker({
            format: "mm/dd/yyyy",
            clearBtn: true,
            multidate: true,
            multidateSeparator: ","
        });
        jQuery('#date-range').datepicker({
            toggleActive: true
        });
    },

    AdvanceFormApp.prototype.initDateRangePicker = function() {
        //Date range picker
        $('.input-daterange-datepicker').daterangepicker({
            buttonClasses: ['btn', 'btn-sm'],
            applyClass: 'btn-success',
            cancelClass: 'btn-light'
        });
        $('.input-daterange-timepicker').daterangepicker({
            timePicker: true,
            timePickerIncrement: 30,
            locale: {
                format: 'MM/DD/YYYY h:mm A'
            },
            buttonClasses: ['btn', 'btn-sm'],
            applyClass: 'btn-success',
            cancelClass: 'btn-light'
        });
        $('.input-limit-datepicker').daterangepicker({
            format: 'MM/DD/YYYY',
            minDate: '06/01/2018',
            maxDate: '06/30/2018',
            buttonClasses: ['btn', 'btn-sm'],
            applyClass: 'btn-success',
            cancelClass: 'btn-light',
            dateLimit: {
                days: 6
            }
        });

        $('#reportrange span').html(moment().subtract(29, 'days').format('MMMM D, YYYY') + ' - ' + moment().format('MMMM D, YYYY'));

        $('#reportrange').daterangepicker({
            format: 'MM/DD/YYYY',
            startDate: moment().subtract(29, 'days'),
            endDate: moment(),
            minDate: '01/01/2017',
            maxDate: '12/31/2020',
            dateLimit: {
                days: 60
            },
            showDropdowns: true,
            showWeekNumbers: false,
            timePicker: false,
            timePickerIncrement: 1,
            timePicker12Hour: true,
            ranges: {
                'Today': [moment(), moment()],
                'Yesterday': [moment().subtract(1, 'days'), moment().subtract(1, 'days')],
                'Last 7 Days': [moment().subtract(6, 'days'), moment()],
                'Last 30 Days': [moment().subtract(29, 'days'), moment()],
                'This Month': [moment().startOf('month'), moment().endOf('month')],
                'Last Month': [moment().subtract(1, 'month').startOf('month'), moment().subtract(1, 'month').endOf('month')]
            },
            opens: 'left',
            drops: 'down',
            buttonClasses: ['btn', 'btn-sm'],
            applyClass: 'btn-success',
            cancelClass: 'btn-light',
            separator: ' to ',
            locale: {
                applyLabel: 'Submit',
                cancelLabel: 'Cancel',
                fromLabel: 'From',
                toLabel: 'To',
                customRangeLabel: 'Custom',
                daysOfWeek: ['Su', 'Mo', 'Tu', 'We', 'Th', 'Fr', 'Sa'],
                monthNames: ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'],
                firstDay: 1
            }
        }, function (start, end, label) {
            console.log(start.toISOString(), end.toISOString(), label);
            $('#reportrange span').html(start.format('MMMM D, YYYY') + ' - ' + end.format('MMMM D, YYYY'));
        });
    },

    /** 
     * Initilize
    */
   AdvanceFormApp.prototype.init = function() {
        var $this = this;
        this.initSelect2();
        this.initMaxLength();
        this.initTimePicker();
        this.initColorPicker();
        this.initMask();
        this.initDatePicker();
        this.initDateRangePicker();
    },

    $.AdvanceFormApp = new AdvanceFormApp, $.AdvanceFormApp.Constructor = AdvanceFormApp


}(window.jQuery),
    //initializing main application module
function($) {
    "use strict";
    $.AdvanceFormApp.init();
}(window.jQuery);
