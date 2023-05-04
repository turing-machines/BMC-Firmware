<!DOCTYPE html>
<html lang="zh-CN" class="configure">

<head>

  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">

  <meta name="viewport" content="width=device-width, initial-scale=1">

  <meta http-equiv="Expires" content="0">
  <meta http-equiv="Pragma" content="no-cache">
  <meta http-equiv="Cache-control" content="no-cache">
  <meta http-equiv="Cache" content="no-cache">


  <title>turing config</title>


  <!--[if lt IE 9]>
      <script src="./js/html5shiv.min.js"></script>
      <script src="./js/respond.min.js"></script>
    <![endif]-->


  <link href="css/style.css" rel="stylesheet">
  <link href="css/css.css" rel="stylesheet">




  <!--new css-->
  <link rel="stylesheet" href="css/new/vendor/animate.css/animate.css">
  <link href="css/new/materialdesignicons.min.css" rel="stylesheet" type="text/css" />
  <link href="css/new/webfont.css" rel="stylesheet" type="text/css" />
  <link href="css/new/simple-line-icons.css" rel="stylesheet" type="text/css" />

  <link href="css/new/select2.min.css" rel="stylesheet" type="text/css" />
  <link href="css/new/bootstrap-tagsinput.css" rel="stylesheet" />
  <link href="css/new/switchery.min.css" rel="stylesheet">
  <link href="css/new/bootstrap-select.min.css" rel="stylesheet" />
  <link href="css/new/bootstrap-timepicker.min.css" rel="stylesheet" />
  <link href="css/new/bootstrap-colorpicker.min.css" rel="stylesheet" />
  <link href="css/new/bootstrap-datepicker.min.css" rel="stylesheet" />
  <link href="css/new/daterangepicker.css" rel="stylesheet" />

  <!-- Toastr css -->
  <link href="css/new/jquery.toast.min.css" rel="stylesheet" />

  <!-- ION Slider -->
  <link href="css/new/ion.rangeSlider.css" rel="stylesheet" type="text/css" />
  <link href="css/new/ion.rangeSlider.skinFlat.css" rel="stylesheet" type="text/css" />




  <!-- App css -->
  <!-- build:css -->
  <link href="css/new/app.css" rel="stylesheet" type="text/css" />

  <!--new css end-->

  <!--------------------------------- JQuery.JS ------------------------------------------------->
  <script src="js/jquery.min.js" type="text/javascript"></script>

  <!--------------------------------- slider.js ---------------------------------------------->
  <script src="js/new/ion.rangeSlider.min.js"></script>
  <script src="js/new/jquery.range-sliders.js"></script>

  <!--------------------------------- Fnuction ------------------------------------------------->
  <script src="js/new/m_page_disp.js" type="text/javascript"></script>


  <script type="text/javascript">

    //CB : http req sdcard set result
    function C_getCBResuleParseSDCardDisp(json) {
      console.log(json);

      $.each(json, function (index, item) {
        var total = json[index][0].total;
        var use = json[index][0].use;
        var free = json[index][0].free;

        //
        $("#sdTotal").val(total);
        $("#sdUse").val(use);
        $("#sdFree").val(free);
        console.log('total:' + total);
      });
    }

    function C_getCBResuleParsePowerDisp(json) {
      console.log(json);
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
      console.log(json);
      $.each(json, function (index, item) {
        var mode = json[index][0].mode;
        var node = json[index][0].node;

        //
        // $("#usbMode").val(mode);
        // $("#usbNode").val(node);
        $('#usbMode').selectpicker('val', mode);
        $('#usbNode').selectpicker('val', node);
        console.log('mode:' + mode);
      });

    }


    function C_getCBResuleParseNodeInfoDisp(json) {
      console.log(json);

      $.each(json, function (index, item) {
        var node1 = json[index][0].node1;
        var node2 = json[index][0].node2;
        var node3 = json[index][0].node3;
        var node4 = json[index][0].node4;

        //
        $("#node1type").val(node1);
        $("#node2type").val(node2);
        $("#node3type").val(node3);
        $("#node4type").val(node4);
        // console.log('total:' + total);
      });
    }


    function C_getParamAll() {
      C_getParamList('usb');
      C_getParamList('sdcard');
      C_getParamList('power');
      C_getParamList('nodeinfo');
    }


    function C_getParamList(ltype) {
      if (ltype == 'usb') {
        var url = '/api/bmc?opt=get&type=usb';
      }
      else if (ltype == 'sdcard') {
        var url = '/api/bmc?opt=get&type=sdcard';
      }
      else if(ltype == 'power'){
        var url = '/api/bmc?opt=get&type=power';
      }
      else if(ltype == 'nodeinfo'){
        var url = '/api/bmc?opt=get&type=nodeinfo';
      }
      else
        return;

      page_http_req_get(url, ltype);
    }

    function C_setParamUsb() {
      var mode = $('#usbMode').val();
      var node = $('#usbNode').val();
      var url = '/api/bmc?opt=set&type=usb&mode=' + mode + '&node=' + node + '';
      console.log(url);
      page_http_req_set(url, 'usb');
    }

    function C_setParamPower(){
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

    
    function C_setParamResetNetwork(){
      var url = '/api/bmc?opt=set&type=network&cmd=reset';
      page_http_req_set(url, 'network');
    }


    function C_setParamSdcard() {
      var url = '/api/bmc?opt=set&type=sdcard&mode=format';
      console.log(url);
      page_http_req_set(url, 'sdcard');
    }

    function C_setParamFirmware()
    {
      var url="/api/bmc?opt=set&type=firmware";
      page_http_req_set(url,'firmware');
    
    }


  </script>

</head>

<body onload="C_getParamAll()">

  <!-- 中间部分 -->
  <div class="animated fadeInUp" style="position: absolute; left: 10px; top: 10px; width: 100%;">
    <div class="content_box clearfix">
      <div class="aui_nav_content_box">
        <div class="aui_nav_list_box">
          <ul class="clearfix">
            <li class="aui_current">
              <a href="javascript:;">usb</a>
            </li>
            <li class="aui_current">
              <a href="javascript:;">power</a>
            </li>
            <li class="aui_current">
              <a href="javascript:;">nodeInfo</a>
            </li>
            <li>
              <a href="javascript:;">sdcard</a>
            </li>
            <li>
              <a href="javascript:;">other</a>
            </li>
            <li>
              <a href="javascript:;">update</a>
            </li>
          </ul>
        </div>


        <div class="aui_nav_list_tab">
          <div class="aui_nav_list_item">
            <ul>
              <div class="row">
                <div class="col-lg-6">

                  <form role="form" id="form1">

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">usb mode:</label>
                      <div class="col-sm-7">
                        <select id="usbMode" class="selectpicker" data-style="btn-outline-primary">
                          <option value="0">Host</option>
                          <option value="1">Device</option>
                        </select>
                      </div>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">connet node:</label>
                      <div class="col-sm-7">
                        <select id="usbNode" class="selectpicker" data-style="btn-outline-primary">
                          <option value="0">node1</option>
                          <option value="1">node2</option>
                          <option value="2">node3</option>
                          <option value="3">node4</option>
                        </select>
                      </div>
                    </div>

                    <div class="form-group row">
                      <div class="col-sm-12 " style="text-align: left;left: 250px;">
                        <button type="submit" class="btn btn-outline-danger btn-rounded w-md">Submit</button>
                        <!-- <button type="reset" class="btn btn-outline-secondary btn-rounded w-md">重置</button> -->
                      </div>
                    </div>
                  </form>
                </div>
              </div>
            </ul>
          </div>

          <div class="aui_nav_list_item" style="display: none;">
            <ul>
              <div class="row">
                <div class="col-lg-6">

                  <form role="form" id="form3">
                    <label >Control the power supply of the connected node!</label>
                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node1:</label>
                      <div class="col-sm-7">
                        <div class="switchery-demo">
                          <input type="checkbox" id="node1Power" data-plugin="switchery" data-color="#008000"
                            data-size="small" />
                        </div>
                      </div>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node2:</label>
                      <div class="col-sm-7">
                        <div class="switchery-demo">
                          <input type="checkbox" id="node2Power" data-plugin="switchery" data-color="#008000"
                            data-size="small" />
                        </div>
                      </div>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node3:</label>
                      <div class="col-sm-7">
                        <div class="switchery-demo">
                          <input type="checkbox" id="node3Power" data-plugin="switchery" data-color="#008000"
                            data-size="small" />
                        </div>
                      </div>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node4:</label>
                      <div class="col-sm-7">
                        <div class="switchery-demo">
                          <input type="checkbox" id="node4Power" data-plugin="switchery" data-color="#008000"
                            data-size="small" />
                        </div>
                      </div>
                      
                    </div>

                    <div class="form-group row">
                      <div class="col-sm-12 " style="text-align: left;left: 250px;">
                        <button type="submit" class="btn btn-outline-danger btn-rounded w-md">Submit</button>
                        <!-- <button type="reset" class="btn btn-outline-secondary btn-rounded w-md">重置</button> -->
                      </div>
                    </div>
                  </form>
                </div>
              </div>
            </ul>
          </div>

          <!-- node info -->

          <div class="aui_nav_list_item" style="display: none;">
            <ul>
              <div class="row">
                <div class="col-lg-6">

                  <form role="form" id="nodetype">
                    <!-- <label >TIPS:Enable to obtain node information using serial port!</label>
                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">enable:</label>
                      <div class="col-sm-7">
                        <div class="switchery-demo">
                          <input type="checkbox" id="nodeInfoEn" data-plugin="switchery" data-color="#008000"
                            data-size="small" />
                        </div>
                      </div>
                    </div> -->

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node1:</label>
                      <div class="col-sm-7">
                        <input type="text" id="node1type" class="form-control" disabled="true" />
                      </div>
                      <!-- <label for="inputEmail3" id="node1type" class="col-sm-2 col-form-label">unknown</label> -->
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node2:</label>
                      <div class="col-sm-7">
                        <input type="text" id="node2type" class="form-control" disabled="true" />
                      </div>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node3:</label>
                      <div class="col-sm-7">
                        <input type="text" id="node3type" class="form-control" disabled="true" />
                      </div>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">node4:</label>
                      <div class="col-sm-7">
                        <input type="text" id="node4type" class="form-control" disabled="true" />
                      </div>
                    </div>

                   

                    <!-- <div class="form-group row">
                      <div class="col-sm-12 " style="text-align: left;left: 250px;">
                        <button type="submit" class="btn btn-outline-danger btn-rounded w-md">Submit</button>
                      </div>
                    </div> -->
                  </form>
                </div>
              </div>
            </ul>
          </div>  

          <!--      以下为  sdcard    -->

          <div class="aui_nav_list_item" style="display: none;">
            <ul>
              <div class="row">
                <div class="col-lg-6">
                  <form role="form" id="form2" ">

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">total:</label>
                      <div class="col-sm-7">
                        <input type="text" id="sdTotal" class="form-control" disabled="true" />
                      </div>
                      <label>MB</label>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">use:</label>
                      <div class="col-sm-7">
                        <input type="text" id="sdUse" class="form-control" disabled="true" />
                      </div>
                      <label>MB</label>
                    </div>

                    <div class="form-group row">
                      <label for="inputEmail3" class="col-sm-2 col-form-label">free:</label>
                      <div class="col-sm-7">
                        <input type="text" id="sdFree" class="form-control" disabled="true" />
                      </div>
                      <label>MB</label>
                    </div>

                    <!-- <div class="form-group row">
                      <div class="col-sm-12 " style="text-align: left;left: 250px;">
                        <button type="submit" class="btn btn-outline-danger btn-rounded w-md">format</button>
                      </div>
                    </div> -->
                  </form>
                </div>
              </div>
            </ul>
          </div>
<!-- other  -->
          <div class="aui_nav_list_item" style="display: none;">
            <ul>
              <div class="row">
                <div class="col-lg-6">

                  <form role="form" id="form=4">
                    <div class="form-group row">
                      <div class="col-sm-12 " style="text-align: left;left: 250px;">
                        <button type="submit" class="btn btn-outline-danger btn-rounded w-md">reset network</button>
                        <!-- <button type="reset" class="btn btn-outline-secondary btn-rounded w-md">重置</button> -->
                      </div>
                    </div>
                  </form>

                </div>
              </div>
            </ul>
          </div>
<!-- update  -->
          <div class="aui_nav_list_item" style="display: none;">
            <ul>
              <div class="row">
                <div class="col-lg-6">

                  <form method="post" action="/api/bmc?opt=set&type=firmware" enctype="multipart/form-data">
                    <input type="hidden">
                    <table border="0">
                        <tr><td>upgrade File:</td><td><input type="file" name="file"></td></tr>
                    </table>
                    <div class="form-group row">
                      <div class="col-sm-12 " style="text-align: left;left: 250px;">
                        <button type="submit" class="btn btn-outline-danger btn-rounded w-md">Submit</button>
                      </div>
                    </div>
                </form>

                <!-- <hr style="height:1px;border:none;border-top:1px dashed #666;" />
                <br>
                <div class="form-group row">
                  <label for="inputEmail3"  class="col-sm-2 col-form-label" ><script type="text/javascript">SYSTEM_DEVICE_UPGRADE</script></label>
                  <div class="col-sm-7">
                    <button type="button" onclick="C_OnclickSetUpdate();" class="btn btn-outline-danger w-md"><script type="text/javascript">SYSTEM_UPGRADE_FILE</script></button>
                  </div> 
                </div> -->
              
                <!-- <form role="form" id="form5">
                  <label >Control !</label>
                  <div class="form-group row">
                    <label for="inputEmail3" class="col-sm-2 col-form-label">node1:</label>
                    <div class="col-sm-7">
                      <div class="switchery-demo">
                        <input type="file" name="file"  data-color="#008000"
                          data-size="small" />
                      </div>
                    </div>
                  </div>

                  <div class="form-group row">
                    <div class="col-sm-12 " style="text-align: left;left: 250px;">
                      <button type="submit" class="btn btn-outline-danger btn-rounded w-md">Submit</button>
                    </div>
                  </div>
                </form> -->

                </div>
              </div>
            </ul>
          </div>

        </div>
      </div>
    </div>
  </div>


  <!-- 中间结束 -->

  <!--------------------------------- JS ------------------------------------------------->
  <!--  <script src="js/jquery.min.js"></script> -->
  <script src="js/bootstrap.min.js"></script>


  <!--new js-->
  <!-- jQuery  -->
  <script src="js/new/bootstrap.bundle.min.js"></script>
  <script src="js/new/jquery.slimscroll.min.js"></script>



  <!-- select2 js -->
  <script src="js/new/select2.min.js"></script>
  <script src="js/new/bootstrap-tagsinput.min.js"></script>
  <script src="js/new/switchery.min.js"></script>
  <script src="js/new/bootstrap-maxlength.min.js"></script>


  <!-- Mask input -->
  <script src="js/new/jquery.mask.min.js"></script>


  <!-- Bootstrap Select -->
  <script src="js/new/bootstrap-select.min.js"></script>
  <script src="js/new/bootstrap-timepicker.min.js"></script>
  <script src="js/new/bootstrap-colorpicker.min.js"></script>
  <script src="js/new/moment.js"></script>
  <script src="js/new/bootstrap-datepicker.min.js"></script>
  <script src="js/new/daterangepicker.js"></script>

  <!-- Toastr js -->
  <script src="js/new/jquery.toast.min.js"></script>
  <script src="js/new/jquery.toastr.js"></script>

  <!-- Init Js file -->
  <script src="js/new/jquery.form-advanced.js"></script>


  <!-- Parsley js -->
  <script type="text/javascript" src="js/new/parsley.min.js"></script>


  <script src="js/new/jquery.core.js"></script>
  <script src="js/new/jquery.app.js"></script>
  <!--new js end-->



  <script>
    $(function () {
      var num = sessionStorage.getItem('fromAVNum');
      $('.aui_nav_list_box').find('li').eq(num).addClass('aui_current').siblings().removeClass('aui_current');
      $('.aui_nav_list_tab>div:eq(' + num + ')').show().siblings().hide();
      $('.aui_nav_list_box li').click(function () {
        sessionStorage.setItem('fromAVNum', $(this).index());
        $(this).addClass('aui_current').siblings().removeClass('aui_current');
        $('.aui_nav_list_tab>div:eq(' + $(this).index() + ')').show().siblings().hide();
      })
    })
  </script>


  <script type="text/javascript">

    $('#form1').parsley();
    $('#form2').parsley();
    $("#form1").submit(function (e) {
      C_setParamUsb();
    });
    $("#form2").submit(function (e) {
      C_setParamSdcard();
    });
    $("#form3").submit(function (e) {
      C_setParamPower();
    });
    $("#form4").submit(function (e) {
      C_setParamResetNetwork();
    });
    $("#form5").submit(function (e) {
      C_setParamFirmware();
    });

    C_Notification();

  </script>

</body>

</html>
