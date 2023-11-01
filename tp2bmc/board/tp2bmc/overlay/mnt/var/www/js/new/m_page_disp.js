function page_changeSwitchery(element, checked) {
    if ((element.is(':checked') && checked == false) || (!element.is(':checked') && checked == true)) {
        element.parent().find('.switchery').trigger('click');
    }
}

function bool2int(val) {
    if (val == false)
        return 0;
    else
        return 1;
}

function oth2bool(val) {
    if (val == 0)
        return false;
    else
        return true;
}

function C_setCBResuleParseDisp(json) {
    var Result;
    if (json == "urlerr") {
        sessionStorage.setItem('Notification', 'err');
    } else {
        $.each(json, function (index, item) {
            Result = json[index][0].result;
        });

        if (Result == "ok") {
            sessionStorage.setItem('Notification', 'ok');
        } else {
            sessionStorage.setItem('Notification', 'err');
        }
    }
}

function C_Notification() {
    if (sessionStorage.getItem('Notification') == "ok") {
        $.NotificationApp.send("info", "save ok .", 'top-center', '#1ea69a', 'info', 2000, 1, 'slide');
    } else if (sessionStorage.getItem('Notification') == "err") {
        $.NotificationApp.send("info", "save err.", 'top-center', '#1ea69a', 'error', 2000, 1, 'slide');
    }

    sessionStorage.removeItem('Notification');
    //sessionStorage.clear();       // modify by wenyi
}

function page_http_req_get(uUrl, lType) {
    $.ajax({
        url: '' + uUrl + '',
        type: 'GET',
        dataType: 'text',
        timeout: 5000,
        cache: false,
        error: erryFunction,
        success: succFunction
    })

    function erryFunction() {
        alert("page get error");
    }

    function succFunction(uStr) {
        var response = JSON.parse(uStr);
        var json = response["response"][0];

        if (lType == 'usb')
            C_getCBResuleParseUSBDisp(json);
        else if (lType == 'sdcard')
            C_getCBResuleParseSDCardDisp(json);
        else if (lType == 'other')
            C_getCBResuleParseOtherDisp(json);
        else if (lType == 'power')
            C_getCBResuleParsePowerDisp(json);
        else if (lType == 'nodeinfo')
            C_getCBResuleParseNodeInfoDisp(json);
    }
}

function page_http_req_set(uUrl, lType) {
    $.ajax({
        url: '' + uUrl + '',
        type: 'GET',
        dataType: 'text',
        timeout: 5000,
        cache: false,
        async: false,
        error: erryFunction,
        success: succFunction
    })

    function erryFunction() {
        console.log("ajax post error");
        C_setCBResuleParseDisp("urlerr");
    }

    function succFunction(uStr) {
        var json = JSON.parse(uStr);
        C_setCBResuleParseDisp(json);
    }
}

function C_getCBResuleParseSDCardDisp(json) {
    $.each(json, function (index, item) {
        var total = json[index][0].total;
        var use = json[index][0].use;
        var free = json[index][0].free;

        $("#sdTotal").val(total);
        $("#sdUse").val(use);
        $("#sdFree").val(free);
    });
}

function C_getCBResuleParseOtherDisp(json) {
    $.each(json, function (index, item) {
        var version = json[index][0].version;
        var buildtime = json[index][0].buildtime;
        var ip = json[index][0].ip;
        var mac = json[index][0].mac;

        $("#otherVer").text(version);
        $("#otherBuildtime").text(buildtime);
        $("#otherIp").text(ip);
        $("#otherMac").text(mac);
    });
}

function C_getCBResuleParsePowerDisp(json) {
    $.each(json, function (index, item) {
        var node1 = json[index][0].node1;
        var node2 = json[index][0].node2;
        var node3 = json[index][0].node3;
        var node4 = json[index][0].node4;

        var element1 = $('#node1Power');
        page_changeSwitchery(element1, oth2bool(node1));

        var element2 = $('#node2Power');
        page_changeSwitchery(element2, oth2bool(node2));

        var element3 = $('#node3Power');
        page_changeSwitchery(element3, oth2bool(node3));

        var element4 = $('#node4Power');
        page_changeSwitchery(element4, oth2bool(node4));
    });
}

function C_getCBResuleParseUSBDisp(json) {
    $.each(json, function (index, item) {
        var mode = json[index][0].mode;
        var node = json[index][0].node;

        $('#usbMode').selectpicker('val', mode);
        $('#usbNode').selectpicker('val', node);
    });
}

function C_getCBResuleParseNodeInfoDisp(json) {
    $.each(json, function (index, item) {
        var node1 = json[index][0].node1;
        var node2 = json[index][0].node2;
        var node3 = json[index][0].node3;
        var node4 = json[index][0].node4;

        $("#node1type").val(node1);
        $("#node2type").val(node2);
        $("#node3type").val(node3);
        $("#node4type").val(node4);
    });
}

function C_getParamAll() {
    C_getParamList('usb');
    C_getParamList('sdcard');
    C_getParamList('power');
    C_getParamList('nodeinfo');
    C_getParamList('other');
}

function C_getParamList(ltype) {
    if (ltype == 'usb') {
        var url = '/api/bmc?opt=get&type=usb';
    } else if (ltype == 'sdcard') {
        var url = '/api/bmc?opt=get&type=sdcard';
    } else if (ltype == 'power') {
        var url = '/api/bmc?opt=get&type=power';
    } else if (ltype == 'nodeinfo') {
        var url = '/api/bmc?opt=get&type=nodeinfo';
    } else if (ltype == 'other') {
        var url = '/api/bmc?opt=get&type=other';
    } else
        return;

    page_http_req_get(url, ltype);
}

function C_setParamUsb() {
    var mode = $('#usbMode').val();
    var node = $('#usbNode').val();
    var url = '/api/bmc?opt=set&type=usb&mode=' + mode + '&node=' + node + '';
    page_http_req_set(url, 'usb');
}

function C_setParamPower() {
    var node1 = $('#node1Power').is(':checked');
    var node2 = $('#node2Power').is(':checked');
    var node3 = $('#node3Power').is(':checked');
    var node4 = $('#node4Power').is(':checked');

    node1 = bool2int(node1);
    node2 = bool2int(node2);
    node3 = bool2int(node3);
    node4 = bool2int(node4);

    var url = '/api/bmc?opt=set&type=power&node1=' + node1 + '&node2=' + node2 + '&node3=' + node3 + '&node4=' + node4 + '';
    page_http_req_set(url, 'power');
}

function C_setParamResetNetwork() {
    var url = '/api/bmc?opt=set&type=network&cmd=reset';
    page_http_req_set(url, 'network');
}

function C_setParamSdcard() {
    var url = '/api/bmc?opt=set&type=sdcard&mode=format';
    page_http_req_set(url, 'sdcard');
}

function C_setParamFirmware() {
    var url = "/api/bmc?opt=set&type=firmware";
    page_http_req_set(url, 'firmware');
}

function load_button_toggle_behavior(input, button) {
    function toggle() {
        if ($(input)[0].files.length > 0) {
            $(button).removeAttr("disabled");
        } else {
            $(button).attr("disabled");
        }
    }

    toggle();

    $(input).on('change', function () {
        toggle();
    });
}

function get_request_handle(request_url) {
    return new Promise(function (resolve) {
        $.ajax({
            url: request_url,
            type: 'GET',
            timeout: 5000,
            dataType: "json",
            success: function (data) {
                resolve(data["handle"]);
            }
        });
    });
}

function get_status(type) {
    return new Promise(function (resolve, reject) {
        var request_status = '/api/bmc?opt=get&type=' + type;
        $.ajax({
            url: request_status,
            type: 'GET',
            timeout: 5000,
            dataType: "json",
            success: function (data) {
                if ("Error" in data) {
                    reject(data["Error"]);
                } else {
                    resolve(data);
                }
            },
            error: function (error) {
                reject(error);
            },
        });
    });
}

function multipart_transfer(handle, form_data, progress_bar) {
    return new Promise(function (resolve, reject) {
        $.ajax({
            url: "/api/bmc/upload/" + handle,
            type: 'POST',
            data: form_data,
            processData: false,
            contentType: false,
            error: function (jqXHR, textStatus, errorThrown) {
                reject(jqXHR.responseText);
            },
            success: function (data) {
                resolve(data);
            },
            xhr: function () {
                const xhr = new XMLHttpRequest();
                // Add a progress callback for upload
                xhr.upload.addEventListener("progress", function (e) {
                    if (e.lengthComputable) {

                        setTimeout(() => {
                            const percentComplete = (e.loaded / e.total) * 100;
                            $(progress_bar).attr('aria-valuenow', percentComplete);

                            // Width of progressbar
                            $(progress_bar).animate({width: `${percentComplete}%`}, 350);

                            // Caption of progressbar
                            $(progress_bar).text(`${percentComplete}%`)


                            console.log("Upload progress: " + percentComplete + "%");
                        }, 100)


                    }
                }, false);

                return xhr;
            }
        });
    });
}

const sleep = (delay) => new Promise((resolve) => setTimeout(resolve, delay))

async function wait_for_done(type) {
    let state = await get_status(type);
    while (!("Done" in state)) {
        await sleep(1000);
        console.log(state);
        state = await get_status(type);
    }
    console.log(state);
}

async function upload_multipart_action(form, progress_group, type) {
    let update_label = $(progress_group + " label");
    $(form).on("submit", function (event) {
        event.preventDefault();
        $(progress_group).css('display', 'inline');

        var request_transfer;
        var file = $(form + " input")[0].files[0];
        if (type === "flash") {
            var node = $("#node-upgrade-picker").val();
            request_transfer = '/api/bmc?opt=set&type=flash&file=' + file.name + '&length=' + file.size + '&node=' + node;
        } else if (type === "firmware") {
            request_transfer = '/api/bmc?opt=set&type=firmware&file=' + file.name + '&length=' + file.size
        } else {
            alert(type + " is not a option");
        }

        get_request_handle(request_transfer).then(function (handle) {
            var formData = new FormData();
            formData.append('file', file);
            update_label.text("Transfering..");
            return multipart_transfer(handle, formData, progress_group + " .progress-bar");
        }).then(function () {
            update_label.text("verify checksum, and finalizing upgrade..");
            return wait_for_done(type);
        }).then(function () {
            update_label.text("Upgrade completed successful!");
        }).catch(function (err) {
            update_label.text("Upgrade failed: " + err);
        });
    });
}


$(document).ready(function () {
    load_button_toggle_behavior("#node-upgrade-file-id", "#node-upgrade-form button");
    load_button_toggle_behavior("#firmware-upgrade-file-id", "#firmware-upgrade-form button");
    upload_multipart_action("#node-upgrade-form", "#node-progress-group", "flash");
    upload_multipart_action("#firmware-upgrade-form", "#firmware-progress-group", "firmware");
});
