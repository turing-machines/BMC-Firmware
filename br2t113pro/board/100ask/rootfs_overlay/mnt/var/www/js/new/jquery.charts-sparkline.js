/*
Template Name: Greeva - Responsive Bootstrap 4 Admin Dashboard
Author: CoderThemes
File: Sparkline chart
*/

$( document ).ready(function() {
    
    var DrawSparkline = function() {
        $('#sparkline1').sparkline([0, 23, 43, 35, 44, 45, 56, 37, 40], {
            type: 'line',
            width: "100%",
            height: '165',
            chartRangeMax: 50,
            lineColor: '#188ae2',
            fillColor: 'rgba(24, 138, 226, 0.3)',
            highlightLineColor: 'rgba(0,0,0,.1)',
            highlightSpotColor: 'rgba(0,0,0,.2)',
            maxSpotColor:false,
            minSpotColor: false,
            spotColor:false,
            lineWidth: 1
        });

        $('#sparkline1').sparkline([25, 23, 26, 24, 25, 32, 30, 24, 19], {
            type: 'line',
            width: "100%",
            height: '165',
            chartRangeMax: 40,
            lineColor: '#f1556c',
            fillColor: 'rgba(229, 43, 76, 0.3)',
            composite: true,
            highlightLineColor: 'rgba(0,0,0,.1)',
            highlightSpotColor: 'rgba(0,0,0,.2)',
            maxSpotColor:false,
            minSpotColor: false,
            spotColor:false,
            lineWidth: 1
        });
    
        $('#sparkline2').sparkline([3, 6, 7, 8, 6, 4, 7, 10, 12, 7, 4, 9, 12, 13, 11, 12], {
            type: 'bar',
            height: '165',
            barWidth: '10',
            barSpacing: '3',
            barColor: '#35b8e0'
        });
        
        $('#sparkline3').sparkline([20, 40, 30, 10], {
            type: 'pie',
            width: '165',
            height: '165',
            sliceColors: ['#35b8e0','#3b3e47','#e3eaef','#f34943']
        });
    
        $('#sparkline4').sparkline([0, 23, 43, 35, 44, 45, 56, 37, 40], {
            type: 'line',
            width: "100%",
            height: '165',
            chartRangeMax: 50,
            lineColor: '#2d7bf4',
            fillColor: 'transparent',
            lineWidth: 2,
            highlightLineColor: 'rgba(0,0,0,.1)',
            highlightSpotColor: 'rgba(0,0,0,.2)',
            maxSpotColor:false,
            minSpotColor: false,
            spotColor:false
        });
    
        $('#sparkline4').sparkline([25, 23, 26, 24, 25, 32, 30, 24, 19], {
            type: 'line',
            width: "100%",
            height: '165',
            chartRangeMax: 40,
            lineColor: '#fbcc5c',
            fillColor: 'transparent',
            composite: true,
            lineWidth: 2,
            maxSpotColor:false,
            minSpotColor: false,
            spotColor:false,
            highlightLineColor: 'rgba(0,0,0,1)',
            highlightSpotColor: 'rgba(0,0,0,1)'
        });

        $('#sparkline6').sparkline([3, 6, 7, 8, 6, 4, 7, 10, 12, 7, 4, 9, 12, 13, 11, 12], {
            type: 'line',
            width: "100%",
            height: '165',
            lineColor: '#e3eaef',
            lineWidth: 2,
            fillColor: 'rgba(227,234,239,0.3)',
            highlightLineColor: 'rgba(0,0,0,.1)',
            highlightSpotColor: 'rgba(0,0,0,.2)'
        });

        $('#sparkline6').sparkline([3, 6, 7, 8, 6, 4, 7, 10, 12, 7, 4, 9, 12, 13, 11, 12], {
            type: 'bar',
            height: '165',
            barWidth: '10',
            barSpacing: '5',
            composite: true,
            barColor: '#f36270'
        });

        $("#sparkline7").sparkline([4, 6, 7, 7, 4, 3, 2, 1, 4, 4, 5, 6, 3, 4, 5, 8, 7, 6, 9, 3, 2, 4, 1, 5, 6, 4, 3, 7], {
            type: 'discrete',
            width: '280',
            height: '165',
            lineColor: '#36404c'
        });

        $('#sparkline8').sparkline([10,12,12,9,7], {
            type: 'bullet',
            width: '280',
            height: '80',
            targetColor: '#64c5b1',
            performanceColor: '#5553ce'
        });
        $('#sparkline9').sparkline([4,27,34,52,54,59,61,68,78,82,85,87,91,93,100], {
            type: 'box',
            width: '280',
            height: '80',
            boxLineColor: '#5553ce',
            boxFillColor: '#f1f1f1',
            whiskerColor: '#32c861',
            outlierLineColor: '#c17d7d',
            medianColor: '#22e535',
            lineWidth: 2,
            targetColor: '#316b1d'
        });
        $('#sparkline10').sparkline([1,1,0,1,-1,-1,1,-1,0,0,1,1], {
            height: '80',
            width: '100%',
            type: 'tristate',
            posBarColor: '#0acf97',
            negBarColor: '#e3eaef',
            zeroBarColor: '#ff679b',
            barWidth: 8,
            barSpacing: 3,
            zeroAxis: false
        });



        },
        DrawMouseSpeed = function () {
            var mrefreshinterval = 500; // update display every 500ms
            var lastmousex=-1; 
            var lastmousey=-1;
            var lastmousetime;
            var mousetravel = 0;
            var mpoints = [];
            var mpoints_max = 30;
            $('html').mousemove(function(e) {
                var mousex = e.pageX;
                var mousey = e.pageY;
                if (lastmousex > -1) {
                    mousetravel += Math.max( Math.abs(mousex-lastmousex), Math.abs(mousey-lastmousey) );
                }
                lastmousex = mousex;
                lastmousey = mousey;
            });
            var mdraw = function() {
                var md = new Date();
                var timenow = md.getTime();
                if (lastmousetime && lastmousetime!=timenow) {
                    var pps = Math.round(mousetravel / (timenow - lastmousetime) * 1000);
                    mpoints.push(pps);
                    if (mpoints.length > mpoints_max)
                        mpoints.splice(0,1);
                    mousetravel = 0;
                   //  alert(mpoints);
                    $('#sparkline5').sparkline(mpoints, {
                        tooltipSuffix: ' %',
                        type: 'line',
                        width: "100%",
                        height: '165',
                        chartRangeMax: 77,
                        maxSpotColor:false,
                        minSpotColor: false,
                        spotColor:false,
                        lineWidth: 1,
                        lineColor: '#31ce77',
                        fillColor: 'rgba(49, 206, 119, 0.3)',
                        highlightLineColor: 'rgba(24,147,126,.1)',
                        highlightSpotColor: 'rgba(24,147,126,.2)'
                    });

                     $('#sparkline55').sparkline(mpoints, {
                        tooltipSuffix: ' %',
                        type: 'line',
                        width: "100%",
                        height: '165',
                        chartRangeMax: 77,
                        maxSpotColor:false,
                        minSpotColor: false,
                        spotColor:false,
                        lineWidth: 1,
                        lineColor: '#31ce77',
                        fillColor: 'rgba(49, 206, 119, 0.3)',
                        highlightLineColor: 'rgba(24,147,126,.1)',
                        highlightSpotColor: 'rgba(24,147,126,.2)'
                    });
                }
                lastmousetime = timenow;
                setTimeout(mdraw, mrefreshinterval);
            }
            // We could use setInterval instead, but I prefer to do it this way
            setTimeout(mdraw, mrefreshinterval); 
        };
    
    DrawSparkline();
    DrawMouseSpeed();
    
    var resizeChart;

    $(window).resize(function(e) {
        clearTimeout(resizeChart);
        resizeChart = setTimeout(function() {
            DrawSparkline();
            DrawMouseSpeed();
        }, 300);
    });
});