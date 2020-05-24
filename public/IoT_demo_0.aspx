<%@ Page Language="C#" %>

<!DOCTYPE html>

<script runat="server">

</script>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title></title>
    
    <link href="./Content/jquery.mobile-1.4.5.css" rel="stylesheet" />
    <style>
        .cen {
            text-align:center;
        }
        .auto-style1 {
            font-size: large;
        }
        .auto-style2 {
            font-size: xx-large;
            font-weight: bold;
        }
    </style>
    
    <style>
.slider {
  -webkit-appearance: none;
  width: 100%;
  height: 15px;
  border-radius: 5px;
  background: #d3d3d3;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}

.slider:hover {opacity: 1;}

.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 25px;
  height: 25px;
  border-radius: 50%;
  cursor: pointer;
}

.slider::-moz-range-thumb {
  width: 25px;
  height: 25px;
  border-radius: 50%;
  background: #4CAF50;
  cursor: pointer;
}
#redSlider::-webkit-slider-thumb {background: red;}
#redSlider::-moz-range-thumb {background: red;}
#greenSlider::-webkit-slider-thumb {background: green;}
#greenSlider::-moz-range-thumb {background: green;}
#blueSlider::-webkit-slider-thumb {background: blue;}
#blueSlider::-moz-range-thumb {background: blue;}
</style>

  <script src="https://www.gstatic.com/firebasejs/4.13.0/firebase.js"></script>
<script src="https://cdnjs.cloudflare.com/ajax/libs/socket.io/2.0.3/socket.io.js"></script> <!-- include socket.io client side script -->
    <script src="./Scripts/jquery-2.1.4.js"></script>
    <script src="./Scripts/jquery.mobile-1.4.5.js"></script>
        <script>
            $(function () {
                $(document).on('swipeleft', "#Page1",
                  function (event) {
                      $.mobile.changePage("#Page2", { transition: "slide" });
                  });
                $(document).on('swiperight', "#Page2",
                 function (event) {
                     $.mobile.changePage("#Page1", { transition: "slide", reverse: true });
                 });
            });
    </script>
    
<script>
//load socket.io-client and connect to the host that serves the page
var socket = io(); 
window.addEventListener("load", function(){ //when page loads
  initFirebaseModule();
  initBlubClickModule();
});

function initBlubClickModule(){
  var ledImage = document.getElementById("ledImage"); 
  var ledImage2 = document.getElementById("ledImage2"); 
  ledImage.addEventListener("click", led1_switchImage);
  ledImage2.addEventListener("click", led2_switchImage);

  function led1_switchImage(){
     var str = document.getElementById("ledImage").src;
	 if(str.includes("image_52a.gif")){
		document.getElementById("ledImage").src = "http://120.125.80.113/jpyuImages/image_52b.gif";
        //send button status to server (as 1 or 0)
        socket.emit("lightFromClient", Number(1)); 
	 }else{
		document.getElementById("ledImage").src = "http://120.125.80.113/jpyuImages/image_52a.gif";
        //send button status to server (as 1 or 0)
        socket.emit("lightFromClient", Number(0)); 
	 }
  }
  
  function led2_switchImage(){
     var str = document.getElementById("ledImage2").src;
	 if(str.includes("r8_yellow.gif")){
		document.getElementById("ledImage2").src = "http://120.125.80.113/jpyuImages/r8_while.gif"; 
        //send button status to server (as 1 or 0)
        socket.emit("light2FromClient", Number(0)); 
	 }else{
		document.getElementById("ledImage2").src = "http://120.125.80.113/jpyuImages/r8_yellow.gif";
        //send button status to server (as 1 or 0)
        socket.emit("light2FromClient", Number(1)); 
	 }
  }
}

function initFirebaseModule(){

			var config;
			var db;
			var Led01;
			var Led02;
			  //==================================
			  // Initialize Firebase
			  config = {
				apiKey: "AIzaSyCPUWMlSOG-q35oGsdI0YrU0H-WL0QvST4",
				authDomain: "jpyuhomeautomation.firebaseapp.com",
				databaseURL: "https://jpyuhomeautomation.firebaseio.com",
				projectId: "jpyuhomeautomation",
				storageBucket: "jpyuhomeautomation.appspot.com",
				messagingSenderId: "1019126990182"
			  };
			  firebase.initializeApp(config);
			  db = firebase.database();
			  //==================================
			  
				var ref = db.ref('/Leds/');
				//每次觸發事件(每次Leds節點上的狀態有改變時)都會執行callback function
				ref.on("value", function(snapshot) {
					 console.log(snapshot.val());
					 Led01 = (snapshot.val() && snapshot.val().Led01) || 'Anonymous';
					 Led02 = (snapshot.val() && snapshot.val().Led02) || 'Anonymous';
					 ChangeImage();
					 
					 function ChangeImage() {
						if (Led01 == "1") {
							document.getElementById("ledImage").src = "http://120.125.80.113/jpyuImages/image_52b.gif";
						} else {
							document.getElementById("ledImage").src = "http://120.125.80.113/jpyuImages/image_52a.gif";
						}
						if (Led02 == "1") {
							document.getElementById("ledImage2").src = "http://120.125.80.113/jpyuImages/r8_yellow.gif";
						} else {
							document.getElementById("ledImage2").src = "http://120.125.80.113/jpyuImages/r8_while.gif";
						}
					 }
				}); //ref.on("value"
				

}

</script>
</head>
<body>
    <form id="form1" runat="server">
    <div data-role="page" id="Page1">
        <div data-role="header" data-theme="b">
            <h1 style="text-align: center">
                
                智慧居家控制系統</h1>
        </div>
        <div data-role="content">
            
    <form id="form1">
            <div style="width:100%; margin:0 auto; text-align: center; background-color: #00CC99;" class="auto-style1">
                <h2><b>電燈開關</b></h2>
    </div>
            <div style="width:100%; margin:0 auto; text-align: center; background-color: #FFFF99;">
        <table style="width:300px; margin:0 auto; text-align: center;">
            <tr>
                <td >
    <img id="ledImage" src="http://120.125.80.113/jpyuImages/image_52a.gif" height="160" width="128" />
                </td>
                <td>
    <img id="ledImage2" src="http://120.125.80.113/jpyuImages/r8_while.gif" />
              </td>
            </tr>
        </table>

    </div>
            <div style="width:100%; margin:0 auto; text-align: center; background-color: #00CC99;" class="auto-style1">
                <h2><b>溫溼度資訊</b></h2>
    </div>
            <div style="width:100%; margin:0 auto; text-align: center; background-color: #FFFF99;">
        <table style="width:300px; margin:0 auto; text-align: center;">
            <tr>
                <td class="auto-style2" >
                    溫度 </td>
                <td>
                    
        <label id="temp">26.3&ordm;C </label>
              </td>
            </tr>
            
            <tr>
                <td class="auto-style2" >
                    濕度 </td>
                <td>
                    
        <label id="humd">65.5 </label>
              </td>
            </tr>
        </table>

    </div>
        
            <div style="width:100%; margin:0 auto; text-align: center; background-color: #00CC99;" class="auto-style1">
                <h2><b>三色LED燈</b></h2>
    </div>
            <div style="width:100%; margin:0 auto; text-align: center; background-color: #FFFF99;">
        <table style="width:450px; margin:0 auto; text-align: center;">
            <tr>
                <td  style="width:50px; margin:0 auto; text-align: center;">
    <img id="ledImage" src="http://120.125.80.113/jpyuImages/image_52a.gif" height="160" width="128" />
                </td>
                <td style="width:450px; margin:0 auto; text-align: center;">
                    
<div class="w3-cell-row">
<div >
<p><input type="range" min="0" max="255" value="0" class="slider" id="redSlider"></p>
<p><input type="range" min="0" max="255" value="0" class="slider" id="greenSlider"></p>
<p><input type="range" min="0" max="255" value="0" class="slider" id="blueSlider"></p>
</div>

              </td>
            </tr>
        </table>

    </div>
        <div align= style="text-align: center; " >

        </div>
        </form>
        </div>
        <div data-role="footer" data-theme="b">
            <h1 style="text-align: center">Copyright@2015
                  MCU All Rights Reserved</h1>
        </div>
    </div>

    </form>
</body>
</html>
