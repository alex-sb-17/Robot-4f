const char PAGINA_INDEX[] PROGMEM= R"rawliteral(
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>R4F WebControl</title>

    <style>
       #temperatura, #stare
      {
        height: 100px;
		width: 90%;
        display: flex;
        justify-content: center;
        align-items: center;
        float: center;
        background-color: #f1f1f1;
        border: 1px solid #ccc;
        border-radius: 5px;
        font-size: 32px;
        margin-right: 10px;
        margin-left: 10px;
       }
      
      #tdstyle
      {
		width: 30%;
		justify-content: center;
        align-items: center;
		float: center;
       }
      
      #h2style{
		text-align: center; 
        border-radius: 5px;
        font-size: 28px;
        float: center;
        justify-content: center;
        align-items: center;
      }
      
      #buttons
      {
		width: 90%;
        height: 100px;
        display: flex;
        float: center;
        justify-content: center;
        align-items: center;
        background-color: #f1f1f1;
        border: 1px solid #ccc;
        border-radius: 5px;
        margin-right: 10px;
        margin-left: 10px;
       margin-top: 10px;
       margin-bottom: 10px
      }
    </style>
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js"></script>   
</head>


<body>

 <script>
 
    $(document).ready(function(){
      
     function preiaAfisare(){
       $.ajax({
         url: "returnTemperatura",
         success: function(result) {
            $("#temperatura").html(result + " &#0176C");
         }
       });  
       $.ajax({
         url: "returnStare",
         success: function(result) {
            $("#stare").html(result);
         }
       });

       setTimeout(preiaAfisare, 1000);
     }
     preiaAfisare();
   });
    
 </script>
  
<table>
<tbody>
  <tr>
    <td>
 <div id="temperatura">50</div>
      </td>
    <td ></td>
    <td>
 <div id="stare">50</div>
</td>
  </tr>
  <tr>
  <td style="height:50px;"/>
  </tr>
<tr>
  <td >&nbsp;</td>
  
	<td id=tdstyle >
		<a href="/inainte"><button id=buttons><h2 id=h2style>Inainte</h2></button></a>
	</td>
	<td ">&nbsp;</td>
</tr>
  
<tr height="20"/>

<tr>
	<td id=tdstyle >
		<a href="/stanga"><button id=buttons><h2 id=h2style>Stanga</h2></button></a>
	</td>
		
	<td id=tdstyle>
		<a href="/stop"><button id=buttons> <h2 id=h2style>Stop</h2></button></a>
	</td>
    
	<td id=tdstyle >
	<a href="/dreapta"><button id =buttons><h2 id=h2style>Dreapta</h2></button></a>
	</td>
</tr>
  
<tr height="20"/>
  
<tr>
	<td >&nbsp;</td>
	<td id=tdstyle>
		<a href="/inapoi"> <button id=buttons> <h2 id=h2style>Inapoi</h2></button></a>
	</td>
	<td >&nbsp;</td>
</tr>

</tbody>
</table>
</body>
<html>
)rawliteral";
