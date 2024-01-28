String Style_CSS = R"(
<meta name="viewport"content="width=device-width, initial-scale=1">
<meta http-equiv="content-type"content="text/html; charset=utf-8">
<style>body{margin:0;background-color:#b3ffb3; height:1000px;}
h1{background-color:#004d1a;color:#e6ffee;text-align:left;padding:15px 300px 20px 20px;margin:0;left:0;top:0;position:fixed;width:80%;}
h2{font-family:"Lucida Console","Courier New",monospace;}
table {font-family:arial, san-serif;border-collapse: collapse;width:50%;}
th{background-color:#009933;color:white;}
td, th{border:0px solid #000000;text-align: right;padding: 10px;}
tr:nth-child(even){background-color:#ccffcc;}
tr:nth-child(odd){background-color:#f9ffe5;}
ul{list-style-type:none;margin:0;padding:0;overflow:hidden;background-color:#333;position: fixed;bottom: 0;width: 100%;}
li{float:left;}
li a{display:block;color:white;text-align:center;padding:14px 16px;text-decoration:none;}
li a:hover:not(.active){background-color:#111;}
.active{background-color:#004D1A;}
.button_O{display:inline-block;padding:15px 25px;font-size: 24px;cursor: pointer;text-align: center;text-decoration: none;outline: none;color: #fff;background-color: #FF0000;border: none;border-radius: 15px;box-shadow: 0 9px #999;}
.button_I{display:inline-block;padding:15px 25px;font-size: 24px;cursor: pointer;text-align: center;text-decoration: none;outline: none;color: #fff;background-color: #1f7a1f;border: none;border-radius: 15px;box-shadow: 0 9px #999;}
.button_O:hover{background-color:#b30000}
.button_I:hover{background-color:#103d10}
.button_O:active{background-color:#4d0000;box-shadow: 0 5px #666;transform: translateY(4px);}.button_I:active {background-color: #3e8e41;box-shadow: 0 5px #666;transform: translateY(4px);}
</style>)";



String Home_HTML = R"(
<!DOCTYPE html>
<html>
STYLE_INFO
</head>
<body>
<ul>
<li><a class="active" href="/">Home</a></li>
<li><a href="/boden/">Boden News</a></li>
<li><a href="/toggle/0">Schalten</a></li>
<li><a href="/info/">Info</a></li>
</ul>
<p>_</p><p>_</p>
<h1> DERTAG - DIEUHR </h1>
<h2>- Temperatur Messungen</h2>
<table>
  <tr><th>Temperatur</th><th> Actuell</th><th>  Min   </th><th>  Max   </th></tr>
  <tr><td>{ HEIZEN }   Innen </td><td> IN_TEMP_WERT C</td><td> IN_TEMP_MIN C</td><td> IN_TEMP_MAX C</td></tr>
  <tr><td>Aussen </td><td> AU_TEMP_WERT C</td><td> AU_TEMP_MIN C</td><td> AU_TEMP_MAX C</td></tr> <tr>
  <td>Boden </td><td> BO_TEMP_WERT C</td><td> BO_TEMP_MIN C</td><td> BO_TEMP_MAX C</td></tr></table>  
<canvas id="ChartTemp" style="width: 100%; max-width: 800px; display: block;background-color: #f9ffe5;" width="1600" height="800" class="chartjs-render-monitor"></canvas>
<h2>- Feuchte Messungen</h2>
<table>
  <tr><th> Feuchte</th><th> Actuell</th><th>  Min   </th><th>  Max   </th></tr>
  <tr><td>Innen </td><td> IN_FEUT_WERT %</td><td> IN_FEUT_MIN %</td><td> IN_FEUT_MAX %</td></tr>
 <tr><td>Aussen </td><td> AU_FEUT_WERT %</td><td> AU_FEUT_MIN %</td><td> AU_FEUT_MAX %</td></tr>
 <tr><td>Boden </td><td> BO_FEUT_WERT %</td><td> BO_FEUT_MIN %</td><td> BO_FEUT_MAX %</td></tr>
</table>  
<canvas id="ChartFeut" style="width:100%;max-width:800px;background-color: #f9ffe5;"></canvas>
<h2>- Luftdruck Messungen</h2>
<canvas id="ChartDruck" style="width:100%;max-width:800px;background-color: #f9ffe5;"></canvas>
<p>_</p><p>_</p>
</body>
</html>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js"> </script>
<script>
const xValues = ["0:00","0:10","0:20","0:30","0:40","O:50","1:00","1:10","1:20","1:30","1:40","1:50","2:00","2:10","2:20","2:30","2:40","2:50","3:00","3:10","3:20","3:30","3:40","3:50","4:00","4:10","4:20","4:30","4:40","4:50","5:00","5:10","5:20","5:30","5:40","5:50","6:00","6:10","6:20","6:30","6:40","6:50","7:00","7:10","7:20","7:30","7:40","7:50","8:00","8:10","8:20","8:30","8:40","8:50","9:00","9:10","9:20","9:30","9:40","9:50","10:00","10:10","10:20","10:30","10:40","10:50","11:00","11:10","11:20","11:30","11:40","11:50","12:00","12:10","12:20","12:30","12:40","12:50","13:00","13:10","13:20","13:30","13:40","13:50","14:00","14:10","14:20","14:30","14:40","14:50","15:00","15:10","15:20","15:30","15:40","15:50","16:00","16:10","16:20","16:30","16:40","16:50","17:00","17:10","17:20","17:30","17:40","17:50","18:00","18:10","18:20","18:30","18:40","18:50","19:00","19:10","19:20","19:30","19:40","19:50","20:00","20:10","20:20","20:30","20:40","20:50","21:00","21:10","21:20","21:30","21:40","21:50","22:00","22:10","22:20","22:30","22:40","22:50","23:00","23:10","23:20","23:30","23:40","23:50","24:00"];
const yAuTemp = AU_TEMP_ARRY
const yInTemp = IN_TEMP_ARRY
const yBoTemp = BO_TEMP_ARRY
const yAuFeut = AU_FEUT_ARRY
const yInFeut = IN_FEUT_ARRY
const yBoFeut = BO_FEUT_ARRY
const yLuDruk = LU_DRUK_ARRY
new Chart("ChartTemp", {
 type: "line",
 data: {labels: xValues,datasets:[
 {label:'Aussen Temperatur',data:yAuTemp,pointRadius:2,borderColor:'#000084',fill:false}, 
 {label:'Innen Temperatur',data:yInTemp,pointRadius:2,borderColor:'#ff0000',fill:false},
 {label:'Boden Temperatur',data: yBoTemp,pointRadius: 2,borderColor:"brown",fill:false}]},
 options: {legend: {display:true},scales: {yAxes: [{ticks: {min: -5, max:30}}]}}});
new Chart("ChartFeut", {type: "line",
 data: {labels: xValues,datasets: [
 {label:'Aussen Feuchte',data:yAuFeut,pointRadius: 2,borderColor: '#000084',fill:false}, 
 {label:'Innen Feuchte',data:yInFeut,pointRadius: 2,borderColor: "#ff0000",fill:false},
 {label:'Boden Feuchte',data: yBoFeut,pointRadius: 2,borderColor: "brown",fill: false}]},
 options: {legend: {display:true},scales: {yAxes: [{ticks: {min: 0, max:100}}]}}});
new Chart("ChartDruck", {type: "line",
 data: {labels: xValues,datasets: [
 {label: 'Luftdruck',data: yLuDruk,pointRadius: 2,borderColor: '#1fede3',fill: false}]},
 options: { legend: { display: true },scales: {yAxes: [{ticks: {min: 850, max:1200}}]}}});
</script>)";

String Schalten_HTML = R"(
<!DOCTYPE html>
<html>
<head>
STYLE_INFO
</head>
  <title>Schalten im Gewächshaus</title>
<body>
<ul>
  <li><a class="active" href="/">Home</a></li>
  <li><a href="/boden/">Boden News</a></li>
  <li><a href="/toggle/0">Schalten</a></li>
  <li><a href="/info/">Info</a></li>
</ul>
<p>_</p><p>_</p>
<h1> DERTAG - DIEUHR </h1>
<h2>- Messungen</h2>
<table>
<tr><th>Messung</th><th> Actuell</th><th>  Min   </th> <th>  Max   </th></tr>
<tr><td>Innen Lufttemperatur HEIZEN</td><td> IN_TEMP_WERT C</td><td> IN_TEMP_MIN C</td><td> IN_TEMP_MAX C</td></tr>
<tr><td>Aussen Lufttemperatur</td><td> AU_TEMP_WERT C</td><td> AU_TEMP_MIN C</td><td> AU_TEMP_MAX C</td></tr>
<tr><td>Bodentemperatur I</td><td> BO_TEMP_WERT C</td><td> BO_TEMP_MIN C</td><td> BO_TEMP_MAX C</td></tr>
<tr><td>Bodentemperatur II</td><td> BO_TEMP2_WERT C</td><td> BO_TEMP2_MIN C</td><td> BO_TEMP2_MAX C</td></tr>
<tr><td>Innen Luftfeuchte</td><td> IN_FEUT_WERT %</td><td> IN_FEUT_MIN %</td><td> IN_FEUT_MAX %</td></tr>
<tr><td>Aussen Luftfeuchte</td><td> AU_FEUT_WERT %</td><td> AU_FEUT_MIN %</td><td> AU_FEUT_MAX %</td></tr>
<tr><td>Bodenfeuche I </td><td> BO_FEUT_WERT %</td><td> BO_FEUT_MIN %</td><td> BO_FEUT_MAX %</td></tr>
<tr><td>Bodenfeuche II</td><td> BO_FEUT2_WERT %</td><td> BO_FEUT2_MIN %</td><td> BO_FEUT2_MAX %</td></tr>
<tr><td>LuftDruck</td><td> LU_DRUK_WERT hPa</td><td> LU_DRUK_MIN hPa</td><td> LU_DRUK_MAX hPa</td></tr>
</table>
<h2>- Regelung</h2><table>  <tr>    <th>Regelung    </th>
    <th>ein </th>
    <th>aus </th>
    <th>on/off  </th>
  </tr>
  <tr>
    <td>Termperatur</td>
    <td> 0.0 C </td>
    <td> 1.0 C </a> </td>
    <td style="text-align: center;"><a href="/toggle/5"  class="CLASS_RELAIS5";>RELAIS5_TEXT</a> </td>
  </tr>
  <tr>
    <td>Wasser </td>
    <td> 30.0 % </td>
    <td> 1  min </a> </td>
    <td style="text-align: center;"><a href="/toggle/6"  class="CLASS_RELAIS6";>RELAIS6_TEXT</a> </td>
  </tr>
<table>
<h2>- Relais schalten</h2>
<table>
  <tr >
    <th style="text-align: center;">Heizung     </th>
    <th style="text-align: center;">Pumpe Wasser</th>
    <th style="text-align: center;">Licht No.1  </th>
    <th style="text-align: center;">Licht No.2  </th>
  </tr>
  <tr>
    <td style="text-align: center;"><a href="/toggle/1"  class="CLASS_RELAIS1";>RELAIS1_TEXT</a> </td>
    <td style="text-align: center;"><a href="/toggle/2"  class="CLASS_RELAIS2";>RELAIS2_TEXT</a> </td>
    <td style="text-align: center;"><a href="/toggle/3"  class="CLASS_RELAIS3";>RELAIS3_TEXT</a> </td>
    <td style="text-align: center;"><a href="/toggle/4"  class="CLASS_RELAIS4";>RELAIS4_TEXT</a> </td>
  </tr>
<table>
<p>_</p><p>_</p>
</body>
</html>)";

String Boden_HTML = R"(
<!DOCTYPE html>
<html>
<head>
STYLE_INFO
</head>
<title>Boden Info aus Gewächshaus</title>
<body>
<ul>
  <li><a class="active" href="/">Home</a></li>
  <li><a href="/boden/">Boden News</a></li>
  <li><a href="/toggle/0">Schalten</a></li>
  <li><a href="/info/">Info</a></li>
</ul>
<p>_</p><p>_</p>
<h1> DERTAG - DIEUHR </h1>
<h2>- Messungen</h2>
<table>
<tr><th>Messung</th><th> Actuell</th><th>  Min   </th> <th>  Max   </th></tr>
<tr><td>Bodentemperatur I</td><td> BO_TEMP_WERT C</td><td> BO_TEMP_MIN C</td><td> BO_TEMP_MAX C</td></tr>
<tr><td>Bodentemperatur II</td><td> BO_TEMP2_WERT C</td><td> BO_TEMP2_MIN C</td><td> BO_TEMP2_MAX C</td></tr>
<tr><td>Bodenfeuche I </td><td> BO_FEUT_WERT %</td><td> BO_FEUT_MIN %</td><td> BO_FEUT_MAX %</td></tr>
<tr><td>Bodenfeuche II</td><td> BO_FEUT2_WERT %</td><td> BO_FEUT2_MIN %</td><td> BO_FEUT2_MAX %</td></tr>
</table>
<h2> - Messungen Bodentemperatur </h2>
<canvas id="ChartTemp" style="width: 100%; max-width: 800px; display: block;background-color: #f9ffe5;" width="1600" height="800" class="chartjs-render-monitor"></canvas>
<h2> - Messungen Bodenfeuchte </h2>
<canvas id="ChartFeut" style="width:100%;max-width:800px;background-color: #f9ffe5;"></canvas>
<p>_</p><p>_</p>
<body/>
</html>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js"> </script>
<script>
const xValues = ["0:00","0:10","0:20","0:30","0:40","O:50","1:00","1:10","1:20","1:30","1:40","1:50","2:00","2:10","2:20","2:30","2:40","2:50","3:00","3:10","3:20","3:30","3:40","3:50","4:00","4:10","4:20","4:30","4:40","4:50","5:00","5:10","5:20","5:30","5:40","5:50","6:00","6:10","6:20","6:30","6:40","6:50","7:00","7:10","7:20","7:30","7:40","7:50","8:00","8:10","8:20","8:30","8:40","8:50","9:00","9:10","9:20","9:30","9:40","9:50","10:00","10:10","10:20","10:30","10:40","10:50","11:00","11:10","11:20","11:30","11:40","11:50","12:00","12:10","12:20","12:30","12:40","12:50","13:00","13:10","13:20","13:30","13:40","13:50","14:00","14:10","14:20","14:30","14:40","14:50","15:00","15:10","15:20","15:30","15:40","15:50","16:00","16:10","16:20","16:30","16:40","16:50","17:00","17:10","17:20","17:30","17:40","17:50","18:00","18:10","18:20","18:30","18:40","18:50","19:00","19:10","19:20","19:30","19:40","19:50","20:00","20:10","20:20","20:30","20:40","20:50","21:00","21:10","21:20","21:30","21:40","21:50","22:00","22:10","22:20","22:30","22:40","22:50","23:00","23:10","23:20","23:30","23:40","23:50","24:00"];
const yBoTemp1 = BO_TEMP_ARRY
const yBoTemp2 = BO_TEMP2_ARRY
const yBoFeut1 = BO_FEUT_ARRY
const yBoFeut2 = BO_FEUT2_ARRY
new Chart("ChartTemp", {
 type: "line",
 data: {labels: xValues,datasets:[
 {label:'Boden Temperatur I ',data: yBoTemp1,pointRadius: 2,borderColor:'#b35900',fill:false},
 {label:'Boden Temperatur II',data: yBoTemp2,pointRadius: 2,borderColor:'#ff8000',fill:false}]},
 options: {legend: {display:true},scales: {yAxes: [{ticks: {min: -5, max:30}}]}}});
new Chart("ChartFeut", {type: "line",
 data: {labels: xValues,datasets: [
 {label:'Boden Feuchte I ',data: yBoFeut1,pointRadius: 2,borderColor:'#b35900',fill: false},
 {label:'Boden Feuchte II',data: yBoFeut2,pointRadius: 2,borderColor:'#ff8000',fill: false}]},
 options: {legend: {display:true},scales: {yAxes: [{ticks: {min: 0, max:100}}]}}});
new Chart("ChartDruck", {type: "line",
 data: {labels: xValues,datasets: [
 {label: 'Luftdruck',data: yLuDruk,pointRadius: 2,borderColor: '#1fede3',fill: false}]},
 options: { legend: { display: true },scales: {yAxes: [{ticks: {min: 850, max:1200}}]}}});
</script>)";

String Info_HTML = R"(
<!DOCTYPE html>
<html>
<head>
STYLE_INFO
</head>  
  <title>Info aus Gewächshaus</title>
<body>
<ul>
  <li><a class="active" href="/">Home</a></li>
  <li><a href="/boden/">Boden News</a></li>
  <li><a href="/toggle/0">Schalten</a></li>
  <li><a href="/info/">Info</a></li>
</ul>
<p>_</p><p>_</p>
<h1> DERTAG - DIEUHR </h1>
<h2>- Info </h2>
<table>
  <tr><th>Beschreibung           </th><th> Wert</th>    <th> Bemerkung   </th></tr>
  <tr><td>Wifi Namen             </td><td> WIFINAME </td><td>  </td></tr>
  <tr><td>Wifi Host Namen        </td><td> WIFIHOST </td><td>  </td></tr>
  <tr><td>Wifi Kanale            </td><td> WIFIKANAL</td><td> </td></tr>
  <tr><td>Wifi Stärke            </td><td> WIFIRSSI dBm</td><td> WIFIMAXMINRSSI </td></tr>
  <tr><td>Wifi Abbrüche          </td><td> WIFIERROR </td> <td></td></tr>
  <tr><td>Wifi Abbruchgrund      </td><td> WIFIERR_GR </td>    <td></td></tr>
  <tr><td>ESP IP Adresse         </td><td> ESPIP </td><td></td></tr>
  <tr><td>NTP Server (Zeit)      </td><td> NTPTIME</td><td></td></tr>
  <tr><td>I2C Devices            </td><td> I2CDEV </td><td>I2CANZ</td></tr>
  <tr><td>Störung Sensor innen   </td><td> SBME </td><td></td></tr>
  <tr><td>Störung Sensor aussen  </td><td> SAHT</td><td></td></tr>
  <tr><td>Störung Sensor Licht   </td><td> SNLI </td><td></td></tr> 
  <tr><td>Störung Sensor Dallas  </td><td> SNDALLAS </td><td> Anzahl Senosoren ANZ</td></tr> 
  <tr><td>Störung Power sensor   </td><td> SNPOW </td><td></td></tr>
  <tr><td>Messung Power Volt     </td><td> POWERVOLT </td><td></td></tr>
  <tr><td>Messung Power Strom    </td><td> POWERSTRO </td><td>MINMAXSTRO</td></tr>
  <tr><td>Messung Power Leistung </td><td> POWERLEIS </td><td></td></tr>
  <tr><td>Heitz dauer            </td><td> HEITZDAUER </td><td></td></tr>
  <tr><td>Störung SD - Card      </td><td></td><td></td>  </tr>
  <tr><td>Data SD - Card         </td><td></td><td></td>  </tr>
  <tr><td>Data SD - Card         </td><td></td><td></td>  </tr>
</table>  
<canvas id="ChartStrom" style="width:100%;max-width:800px;background-color: #f9ffe5;"></canvas>
<p>_</p><p>_</p>
</body>
</html>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.js"></script>
<script>
const xValues = ["0:00","0:10","0:20","0:30","0:40","O:50","1:00","1:10","1:20","1:30","1:40","1:50","2:00","2:10","2:20","2:30","2:40","2:50","3:00","3:10","3:20","3:30","3:40","3:50","4:00","4:10","4:20","4:30","4:40","4:50","5:00","5:10","5:20","5:30","5:40","5:50","6:00","6:10","6:20","6:30","6:40","6:50","7:00","7:10","7:20","7:30","7:40","7:50","8:00","8:10","8:20","8:30","8:40","8:50","9:00","9:10","9:20","9:30","9:40","9:50","10:00","10:10","10:20","10:30","10:40","10:50","11:00","11:10","11:20","11:30","11:40","11:50","12:00","12:10","12:20","12:30","12:40","12:50","13:00","13:10","13:20","13:30","13:40","13:50","14:00","14:10","14:20","14:30","14:40","14:50","15:00","15:10","15:20","15:30","15:40","15:50","16:00","16:10","16:20","16:30","16:40","16:50","17:00","17:10","17:20","17:30","17:40","17:50","18:00","18:10","18:20","18:30","18:40","18:50","19:00","19:10","19:20","19:30","19:40","19:50","20:00","20:10","20:20","20:30","20:40","20:50","21:00","21:10","21:20","21:30","21:40","21:50","22:00","22:10","22:20","22:30","22:40","22:50","23:00","23:10","23:20","23:30","23:40","23:50","24:00"];
const yCurrent = STROM_ARRY
new Chart("ChartStrom", {
  type: "line",
  data: { labels: xValues,
    datasets:[{ label:'Strom', data:yCurrent,pointRadius: 1, borderColor: '#000084',fill: false},]},
  options: {legend: {display:true},
    scales: {yAxes: [{ticks: {min: 0, max:1000}}]}}});
</script>
)";  