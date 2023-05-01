$(function(){
	//右侧显示隐藏
	$(".toggle_right").click(function(){
		$('.sidebar').toggle(function() {
			if ($('.sidebar').is(':visible')) {
				$('.sidebar').addClass('collapsed');
			}else {
				$('.sidebar').removeClass('collapsed');
			}
		});	
	});

	//遮罩
	$('.light_btn').click(function() {
		$(this).siblings('.clarty').toggle();
	});

    //流畅优先
    $('.botitle').click(function() {
        $(this).siblings('.clarty_bottom').toggle();
    });
	




// 列表滚动
       var $this = $(".tipmsg");
        var scrollTimer;
        
        //设置鼠标移入清楚定时器,移出时恢复滚动
        $this.hover(function() {
            clearInterval(scrollTimer);
        }, function() {
            scrollTimer = setInterval(function() {
                scrollNews($this);
            }, 1500);
        }).trigger("mouseleave");
 
 
        //设置滚动效果***marinTop的值为负值向上滚动
        function scrollNews(obj) {
            var $self = obj.find(".msgfont");
            var lineHeight = $self.find("li:first").height();
            $self.animate({
                // "scrollTop": -lineHeight + "px"
                "marginTop": -lineHeight +"px"
            }, 800, function() {
                $self.css({
                    marginTop: 0
                }).find("li:first").appendTo($self);//将滚出去的在给添加到列表的尾部实现无限循环滚动
            })
        }


// 滚动条样式
        var $document   = $('.sbar');
        var selector    = '[data-rangeslider]';
        var $inputRange = $(selector);

        function valueOutput(element) {
            var value = element.value;
            var output = element.parentNode.getElementsByTagName('output')[0];
            output.innerHTML = value;
        }

        for (var i = $inputRange.length - 1; i >= 0; i--) {
            valueOutput($inputRange[i]);
        };

        $document.on('input', selector, function(e) {
            valueOutput(e.target);
        });

        $inputRange.rangeslider({
            polyfill: false
        });

        $document.on('click', '#js-example-change-value button', function(e) {
            var $inputRange = $('input[type="range"]', e.target.parentNode);
            var value = $('input[type="number"]', e.target.parentNode)[0].value;

            $inputRange
                .val(value)
                .change();
        });

        $document.on('click', '#js-example-change-attributes button', function(e) {
            var $inputRange = $('input[type="range"]', e.target.parentNode);
            var attributes = {
                min: $('input[name="min"]', e.target.parentNode)[0].value,
                max: $('input[name="max"]', e.target.parentNode)[0].value,
                step: $('input[name="step"]', e.target.parentNode)[0].value
            };

            $inputRange
                .attr(attributes)
                .rangeslider('update', true);
        });

        $document
            .on('click', '#js-example-destroy button[data-behaviour="destroy"]', function(e) {
                $('input[type="range"]', e.target.parentNode).rangeslider('destroy');
            })
            .on('click', '#js-example-destroy button[data-behaviour="initialize"]', function(e) {
                $('input[type="range"]', e.target.parentNode).rangeslider({ polyfill: false });
            });

// 预置点点击

var $div = '';
$div += '<div class="yuzhibtn">';
$div += '<a href="javascript:;" class="icon-64diaoyong"></a>';
$div += '<a href="javascript:;" class="icon-66set"></a>';
$div += '<a href="javascript:;" class="icon-65delet"></a>';
$div += '</div>';

$('.list_video li').each(function(i){

	$(this).one('click',function(){
		$(this).append($div);
	})
})










});// jieshu