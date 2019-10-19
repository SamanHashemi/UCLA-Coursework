var number = 64;
var count = 0;
var count2 = 0;
var hr_list = [0, 0, 0, 0, 64, 65, 64, 65, 65, 66];


setInterval(function() {
	
	$("#bpm").html(hr_list[count]);
    count ++;
}, 1000);


var hr2_list = [94, 93, 93, 92, 93, 92, 91];

setTimeout(function(){
   
   setInterval(function() {
	
	$("#bpm").html(hr2_list[count2]);
    count2 ++;
}, 1000);


}, 37000);



