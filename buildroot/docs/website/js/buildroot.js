function display_activity(result, activity) {
    let loaded = 0;
    let nb_display = 8;
    let container;
    if (result==null) return;
    for (let i = 0; i < result.feed.entry.length; i++) {
        let entry = result.feed.entry[i];
        if (activity==="commit" && entry.title.toString().indexOf("git commit") !== -1) {
            container = document.getElementById("commit-activity");
        } else if (activity==="mailing-list" && entry.title.toString().indexOf("git commit") === -1) {
            container = document.getElementById("mailing-list-activity");
        } else {
            continue;
        }
        loaded += 1
        if (loaded > nb_display)
            break;
        let div = document.createElement("p");
        let link = document.createElement("a");
        let d = new Date(entry.updated);
        let data = '[' + d.toLocaleDateString() + '] ' + entry.title;
        data = data.replace("Re: ","");
        data = data.replace("[Buildroot] ","");
        let text = document.createTextNode(data);
        link.appendChild(text);
        link.title = entry.title;
        link.href = entry.link._href;
        div.appendChild(link);
        container.appendChild(div);
    }
    for (let i = 0; i < (nb_display - loaded); i++) {
        container.appendChild(document.createElement("p"));
    }
}

function load_activity(feedurl) {
    $.ajax({
      url: feedurl
    })
    .done(function(data){
        let x2js = new X2JS();
        let result = x2js.xml_str2json(data.documentElement.outerHTML);
        display_activity(result, "commit");
        display_activity(result, "mailing-list");
    });
}



function google_analytics() {
    let _gaq = _gaq || [];
    _gaq.push(['_setAccount', 'UA-21761074-1']);
    _gaq.push(['_setDomainName', 'none']);
    _gaq.push(['_setAllowLinker', true]);
    _gaq.push(['_trackPageview']);

    let ga = document.createElement('script');
    ga.type = 'text/javascript';
    ga.async = true;
    ga.src = ('https:' === document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    let s = document.getElementsByTagName('script')[0];
    s.parentNode.insertBefore(ga, s);
}

function showTooltip(elem, msg) {
    elem.setAttribute('class', 'btn tooltipped tooltipped-s');
    elem.setAttribute('aria-label', msg);
}

let clipboard = new Clipboard('.btn');

$(function () {
  $('[data-toggle="tooltip"]').tooltip()
});

clipboard.on('success', function(e) {
    e.clearSelection();
    $(e.trigger).tooltip('show');
});

$(function() {
  $('a[href*=\\#]:not([href=\\#])').click(function() {
    if (location.pathname.replace(/^\//,'') === this.pathname.replace(/^\//,'') && location.hostname === this.hostname) {
        let target = $(this.hash);
      target = target.length ? target : $('[name=' + this.hash.slice(1) +']');
      if (target.length) {
        $('html,body').animate({
          scrollTop: target.offset().top
        }, 1000);
        return false;
      }
    }
  });
});

jQuery(document).ready(function($) {
    let url = window.location.href;
    // Get the basename of the URL
    url = url.split(/[\\/]/).pop();
    $('.nav a[href="/' + url + '"]').parent().addClass('active');

    load_activity("/new.atom");

    $('#slides').html('<iframe src="https://docs.google.com/gview?url=http://bootlin.com/doc/training/buildroot/buildroot-slides.pdf&embedded=true" style="position:absolute; width:100%; height:100%; top:0; left:0;" frameborder="0"></iframe>')
});
