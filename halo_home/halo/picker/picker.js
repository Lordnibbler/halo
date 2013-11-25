
var selected_menu ="what";
var preview_selected = "what";
var popup_visible = 0;
var columns = 0;
var selected_light = "picked_color";
var num_lights = 10;
var multiple_selection = 0;
var hexcode_array = new Array('00','11','22','33','44','55','66','77','88','99','AA','BB','CC','DD','EE','FF');
var selected_timelines = new Array();
var saved_timelines = [];

var lights_selected = new Array();

	$(function() {
		var $tab_title_input = $( "#tab_title");
		var $tab_rename_input = $( "#tab_rename");
		var tab_counter = 2;

		// tabs init with a custom tab template and an "add" callback filling in the content
		var $tabs = $( "#tabs").tabs({
			tabTemplate: "<li><a href='#{href}'>#{label}</a> <span class='ui-icon ui-icon-close'>Remove Tab</span></li>",
			add: function( event, ui ) {
				$( ui.panel ).append( '<ul class="sortable"> </ul>' );
				$( ".sortable" ).sortable();
				//$( ".sortable" ).disableSelection();
				$tabs.tabs('select', '#playlist_' + $(ui.tab).text());
				current_playlist = $(ui.tab).text();
				$("#playlist_title").text(current_playlist);
			//	$("#log").html('newly created playlist ' + current_playlist);
				$(ui.tab).dblclick(function(e){
					$dialog_rename.dialog( "open" );
				});
			},
			select: function(event, ui) {
				
				current_playlist = $(ui.tab).text();
				$("#playlist_title").text(current_playlist);
				$("#log").html('newly selected playlist ' + current_playlist);
			},
			remove: function(event, ui) { 
				deletePlaylist($(ui.tab).text());
				$(ui.tab).unbind();
			}
		}).addClass('ui-tabs-vertical ui-helper-clearfix');

		// modal dialog init: custom buttons and a "close" callback reseting the form inside
		var $dialog = $( "#dialog" ).dialog({
			autoOpen: false,
			modal: false,
			position: [410,36],
			buttons: {
				Add: function() {
					addTab();
					$( this ).dialog( "close" );
				},
				Cancel: function() {
					$( this ).dialog( "close" );
				}
			},
			open: function() {
				$tab_title_input.focus();
			},
			close: function() {
				$tab_title_input.val('');
			}
		});
		// modal dialog init: custom buttons and a "close" callback reseting the form inside
		var $dialog_rename = $( "#dialog_rename" ).dialog({
			autoOpen: false,
			modal: false,
			position: [410,36],
			buttons: {
				Change: function() {
					var new_name = $tab_rename_input.val() ;
					var selected = $tabs.tabs('option', 'selected');
					$("#log").html('renaming' + current_playlist+ ' located at '+selected+ ' to '+new_name);
					$('#tabs ul:first li:eq('+selected+') a').text(new_name);
					$('#tabs ul:first li:eq('+selected+') a').attr("href",'#playlist_'+new_name);
					$('#playlist_'+current_playlist).attr("id",'playlist_'+new_name);
					current_playlist = new_name;
					$( this ).dialog( "close" );
				},
				Cancel: function() {
					$( this ).dialog( "close" );
				}
			},
			open: function() {
				$tab_rename_input.focus();
			},
			close: function() {
				$tab_rename_input.val('');
			}
		});

		// addTab form: calls addTab function on submit and closes the dialog
		//var $form = $( "form", $dialog ).submit(function() {
		//	addTab();
	//		$dialog.dialog( "close" );
		//	return false;
//		});

		// actual addTab function: adds new tab using the title input from the form above
		function addTab() {
			var tab_title = $tab_title_input.val() ;
			$tabs.tabs( "add", "#playlist_" + tab_title, tab_title );
			tab_counter++;
		}

		// addTab button: just opens the dialog
		$( "#add_tab" )
			.button()
			.click(function() {
				$dialog.dialog( "open" );
			});
		

		// close icon: removing the tab on click
		// note: closable tabs gonna be an option in the future - see http://dev.jqueryui.com/ticket/3924
		$( "#tabs span.ui-icon-close" ).live( "click", function() {
			var index = $( "li", $tabs ).index( $( this ).parent() );
			$tabs.tabs( "remove", index );
			//$( this ).parent().unbind();
		});
	});



$(document).ready(function(){ 
    LoadNewSceneSelector();
   // localStorage.clear();
   
   
   OpenExistingTimeline("default");
    
    						
	$('html').keyup(function(event) {
  			if (event.keyCode == '13') {
    			 event.preventDefault();
  			 }
		   if(event.which == 16){
		   		multiple_selection =0;
		   }
   
   
			$("#log").html('Handler for .keyup() called, multiple_selection = ' + multiple_selection );
  
	});
		$('html').keydown(function(event) {
  			if (event.keyCode == '13') {
    			 event.preventDefault();
  			 }
		   if(event.which == 16){
		   		multiple_selection =1;
		   }
   			// show the values stored
			

   
			$("#log").html('Handler for .keydown() called, multiple_selection = ' + multiple_selection );
  
	});

    loadPlaylistView();
    loadFileView();	
});

var saving_playlist = "nothing";

function savePlaylist(playlist_to_save){
	
	saving_playlist= "";
	
	$("#playlist_"+ playlist_to_save+" ul li").each(function(index) {
	    saving_playlist = saving_playlist + $(this).attr("title")+",";
	  });
	$("#log").html("saving :"+saving_playlist);
	$.post( "/cgi-bin/scripts/change_light.cgi",{command:"save_playlist", title: playlist_to_save, data: saving_playlist},
		      	function( data ) {
		          	$("#log").html("saved playlist" );
					
		      	}
		    	);
}

function deletePlaylist(playlist_to_delete){
	$("#log").html("deleting :"+playlist_to_delete);
	$.post( "/cgi-bin/scripts/change_light.cgi",{command:"delete_playlist", title: playlist_to_delete},
		      	function( data ) {
		          //	$("#log").html("deleted playlist: "+data );
		      	}
		    	);
	
}

function SaveCurrentTimeLine()
{
	var timeline_title = $("#sequence_title").val();
	if(timeline_title == ''){
		timeline_title = "default";
	}
	$("#log").html(" Saving "+ timeline_title);

	$.post( "/cgi-bin/scripts/change_light.cgi",{command:"save_timeline", title: timeline_title, data: $("#time_line_table").html()},
		      	function( data ) {
		          loadFileView(data);
		      	}
		    	);
	
}

function loadFileView(data){
   
//   data = "sunrise,sunset,rainbow,night,";
	 var names = data.split(',');
      //$("#log").html("loading playlist files: "+(names.length-1));
   
      $("#saved_timeline_list").empty();
    
    for(i = 0; i<names.length - 1;i++){
      $("#saved_timeline_list").append('<div class="selectable" id="file_'+names[i]+'">'+names[i]+'</div>');
    }
	
}
function loadPlaylistView(){
	
	$("#log").html("loading playlist...");
   $("#saved_timeline_list").selectable();
   $.post( "/cgi-bin/scripts/change_light.cgi",{command:"list_timelines"},
               function( data ) {
                 	loadFileView(data);
   
               }
            );	
		 $.post( "/cgi-bin/scripts/change_light.cgi",{command:"read_playlists"},
		               function( data ) {
							
							var playlists = data.split('\*');
							$("#log2").html("size:+"+playlists.length+"t:"+data);
							for(z=0 ;z<playlists.length ; z++){
								$("#log2").append("pl "+i+":" + playlists[z]);
								var list = playlists[z];
								var timelines = list.split(',');
								var title = timelines[0];
								timelines.shift();
								timelines.pop();
								$("#log2").append("playlist:" + title + " ");
							    $("#tabs").tabs( "add", "#playlist_" + title, title );
								addTimelineToPlaylist(title,timelines);
							}
		                 	

		               }
		            );
}

function circle_clicked(e){
	if(multiple_selection == 0){
		for(var i in lights_selected){
			$("#"+i).toggleClass("selected",false); 
	  		delete lights_selected[ i];
	  	}
		
	}													
	if(typeof(lights_selected[ e.target.id]) !== "undefined"){
		$("#"+ e.target.id).toggleClass("selected",false); 
		delete lights_selected[ e.target.id];
		
		
	}else {
		$("#log").html('newly selected ' + e.target.id);
		lights_selected[ e.target.id] = "selected";
		$("#"+ e.target.id).toggleClass("selected",true);
	}
	

}

function value_changed(e){
	var $target = $(e.target);
	var value = $target.val();
	$target.attr("value",value);
	$("#log").html("edited "+ e.target.id+ " "+ value) ;
}

function OpenExistingTimeline(file_to_open){
	
	//SaveCurrentTimeLine();
	
	//$("#time_line_table").empty();
	//$("#time_line_table").append(localStorage.getItem('saved_timeline_'+file_to_open));
    
    if(file_to_open ==="undefined"){
    	$("#sequence_title").val('');
    	$("#time_line_table").empty();
		$('.time_box').unbind();
    	$('.selector_circle').unbind();
		
    }
   	else {
    
    selected_menu ="what";
    preview_selected = "what";
    selected_light = "picked_color";
    $("#sequence_title").val(file_to_open) ;
    
    	$.post( "/cgi-bin/scripts/change_light.cgi",{command:"open_timeline", title: file_to_open},
		      	function( data ) {
		          	$("#time_line_table").empty();
					$("#time_line_table").append(data);
					var restored_columns = $("#time_line_table td").size();
					columns = restored_columns +1;
					$("#log").html("Opened: "+file_to_open );
					
					for(i = 0; i < columns; i ++){
  						$("#light_slot_"+i+"_header").click(function(e){
  		 										
  		 										toggleMenu(e.target.id);
  		 										
	 										  }); 
					}
					$('.time_box').unbind();
				 	 $('.time_box').change(function(e){ value_changed(e);});
				
					$('.selector_circle').toggleClass("selected",false);
					$('.selector_circle').unbind();
					$('.selector_circle').click(function(e){circle_clicked(e);}); 
					
		    	});
		    	
		$("#log").html("Opening: " + file_to_open);
	}
}

function DeleteExistingTimeline(file_to_delete){

	//localStorage.removeItem('saved_timeline_'+file_to_delete);
	//$("#file_"+file_to_delete).remove();
	//localStorage.setItem('saved_timelines',$("#saved_timeline_list").html()); //update file list
	$("#log").html("Deleting: " + file_to_delete);
	$.post( "/cgi-bin/scripts/change_light.cgi",{command:"delete_timeline",title:file_to_delete},
		      	function( data ) {
		      				loadFileView(data);
		      				$("#sequence_title").val('');
    						$("#time_line_table").empty();
							$('.time_box').unbind();
    						$('.selector_circle').unbind();
						    

		        		      	}
		    	);
	

}



function preview_All(column_changed)
{
   if(preview_selected != "what"){
	var column_name = preview_selected.split('_',4);
	//preview_selected.substring(0,preview_selected.search("_header"));
	var light_string = "";
	$("#log").html("Previewing time_slot_" + column_name[2]);
	
   for(var i = 0; i < num_lights; i ++){
		var image = $('#light_slot_'+column_name[2]+'_'+i).css('background-image');
		
		if(image.match(/night.png/)){
		   light_string = light_string + "0,0,0,255\n";
	   }else {
	      var rgb = $('#light_slot_'+column_name[2]+'_'+i).css('background-color');
		   var colors = rgb.match(/[0-9]+/g);
		   light_string = light_string + colors[0]+"," +colors[1]+","+colors[2]+",0\n";
	   }
	}		
	$("#log").html("Previewing time_slot_" + column_name[2]+ " " + light_string);
         $.post( "/cgi-bin/scripts/change_light.cgi",{command:"change_light", data:light_string },
                           function( data ) {
                            $("#log").html("Previewing time_slot_" + column_name[2]+"Done!");
                           }
                        );
   }
}


function colorsAll(r,g,b,v)
{
	$("#log").html("clicked: ");
	for(var i in lights_selected){
		$("#log").append(i+" ");
		if(v == 1){
		    $("#"+i).css('background-image',"url(images/night.png)");
		    $("#"+i).css('background-color','');
		}else {
		   $("#"+i).css('background-image','');
		   $("#"+i).css('background-color', 'rgb('+r+','+g+','+b+')');
		}

  }

  
//  colorCircle(selected_light,RGB);
}


/*function colorCircle(id, RGB)
{	 
	 
	 $("#log").html("clicked: " + id);
	 var ctx = $("#"+id)[0].getContext("2d");
  	 ctx.beginPath();
	 ctx.arc(15, 15, 15, 0, Math.PI*2, false); 
	 ctx.closePath();
	 ctx.fillStyle="#"+RGB;
	 ctx.fill();
}*/

function chooserMode(MODE)
{
   if(MODE == "text") { var RGB = text_rgb; }
   else if(MODE == "link") { var RGB = link_rgb; }
   else if(MODE == "vlink") { var RGB = vlink_rgb; }
   else if(MODE == "alink") { var RGB = alink_rgb; }
   else if(MODE == "back") { var RGB = back_rgb; }
   else if(MODE == "hover") { var RGB = hover_rgb; }


   var RR = RGB.substring(0,2);
   var GG = RGB.substring(2,4);
   var BB = RGB.substring(4,6);

   reColorAndGray(RR,GG,BB);

   //doBGImage();
}


function toggleMenu(name){
	$("#log").html(" name "+ name);
	if(popup_visible && selected_menu == name){ //Hide Menu
	
		$("#column_popup").css('visibility','hidden' );	
		popup_visible  = 0;
	
		$("#log").html(" Removing " + selected_menu+" and popup_vis=" + popup_visible);
	} else { //Display Menu
	
		selected_menu = name;
		
		
		var button_location = 0;
		
		$("#insert_before").unbind();
		$("#insert_after").unbind();
		$("#delete_column").unbind();
		$("#preview").unbind();
		$("#insert_before").css("left",button_location);
		$("#insert_before").css("background-image","url(images/back-32.png)");
		$("#insert_before").click(function(e){
									AddTimeSlot("before",$("#"+selected_menu).parent());
									var position = $("#"+selected_menu).offset();
									var menu_left = position.left + $("#"+selected_menu).outerWidth() / 2 - $("#column_popup").outerWidth() / 2;
									$("#column_popup").css('left',menu_left );
							});
									
		button_location += 	$("#insert_before").outerWidth();
		$("#preview").css("left",button_location);
		$("#preview").css("background-image","url(images/spotlight.png)");
		$("#preview").click(function(e){								
								   
								   if(preview_selected == selected_menu){
									   $("#"+preview_selected).parent().find('.selector_circle').toggleClass("preview_on",false);
							         preview_selected = "what";
							      }
								   else if(preview_selected != selected_menu){
								   $("#"+preview_selected).parent().find('.selector_circle').toggleClass("preview_on",false);
								   $("#"+selected_menu).parent().find('.selector_circle').toggleClass("preview_on",true); 
									   preview_selected = selected_menu;
									}
									
									preview_All(0xFF);
							});
							
		button_location += 	$("#preview").outerWidth();					
		$("#delete_column").css("left",button_location);	
		$("#delete_column").css("background-image",'url(images/table_delete.png)');	
		$("#delete_column").click(function(e){
									$("#"+selected_menu).parent().remove();
									$("#column_popup").css('visibility','hidden' );	
									popup_visible  = 0;	
							});							
		
		button_location += 	$("#delete_column").outerWidth();	
		$("#insert_after").css("left",button_location);	
		$("#insert_after").css("background-image",'url(images/next-add-32.png)');						
		$("#insert_after").click(function(e){
									AddTimeSlot("after",$("#"+selected_menu).parent());
									$("#column_popup").css('visibility','hidden' );	
									popup_visible  = 0;

							});
		
		button_location += 	$("#insert_after").outerWidth();
		
		
		
		$("#column_popup").css('width',button_location );
		$("#column_popup").css('height', $("#insert_after").outerHeight());
		
		var position = $("#"+selected_menu).offset();
		var menu_left = position.left + $("#"+selected_menu).outerWidth() / 2 - $("#column_popup").outerWidth() / 2;
		var menu_top = position.top - $("#column_popup").outerHeight() - 10;
		$("#column_popup").css('top',menu_top);
		$("#column_popup").css('left',menu_left );
		$("#column_popup").css('visibility','visible' );		
		
		popup_visible  = 1;
		$("#log").html("Menu Position: "+"left: " + position.left + ", top: " + position.top +" "+ selected_menu );
		
		
		
	}
}

function AddTimeSlot(order,object)
{
$("#log").html("Submitted: "+ order + " " + object);


  //	$("#time_line_table").append('<td class="scene_column" id="time_slot"> Hello</td>');
  	if(order == "append"){
  	$("#time_line_table").append('<td class="scene_column" id="time_slot_'+columns+'">');
  	}
  	if(order == "before"){
  	object.before('<td class="scene_column" id="time_slot_'+columns+'">');
  	}
  	if(order == "after"){
  	object.after('<td class="scene_column" id="time_slot_'+columns+'">');
  	
  	}
  	
  	$("#time_slot_"+columns).append('<div class="scene_header" id="light_slot_'+columns+'_header">'+columns+'</div>');
  		 $("#light_slot_"+columns+"_header").click(function(e){
  		 										
  		 										toggleMenu(e.target.id);
  		 										
	 										  }); 
  	
  	
  	for(var i in lights_selected){
  		delete lights_selected[ i];
  	}
  for(i = 0; i < num_lights; i ++){
  	$("#time_slot_"+columns).append('<div class="selector_circle" id="light_slot_'+columns+'_'+i+'">&nbsp</div>');
  	$('#light_slot_'+columns+'_'+i).css('background-color',"rgb(0,0,0)");
	
  }  
  $("#time_slot_"+columns).append('<input class="time_box" value="1" type="text" id="time_'+columns+'">');
  $("#time_slot_"+columns).append('<input class="time_box" value="0" type="text" id="fade_'+columns+'">');
  $(".time_box").unbind();
  $('.time_box').change(function(e){value_changed(e);});
  
  $(".selector_circle").unbind();
  $('.selector_circle').click(function(e){
  	circle_clicked(e);
  }); 

  //$("#time_line_table").append('</td>');
  columns ++;
}


function LoadNewSceneSelector()
{
  var num_lights = 10;
  var ctx;
  $("#new_scene_selector").append('<table id="selector_table" cellpadding="0" cellspacing="0">');
  $("#selector_table").append('<td id="light_name_column">');
  $("#light_name_column").append('<div>Time Slot</div>');
  for(i = 0; i < num_lights; i ++){
  	 
  	 $("#light_name_column").append('<div>Light No '+i+'</div>');
  
  }	 
  $("#light_name_column").append('<div>Time(s)</div>');
  $("#light_name_column").append('<div>Fade(ms)</div>');
  $("#selector_table").append('</td>');
  $("#selector_table").append('</table>');

  
  $("#time_line").append('<table id="time_line_table" cellpadding="0" cellspacing="0">');
  
  $("#time_line_table").append('</table>');
 
	genColorTable(0,"hsl");
	$("#color_picker").append('<div class="popup_item" id="night_button" onClick="colorsAll(0,0,0,1)"> </div>');
	$("#night_button").css('background-image','url(images/night.png)');
   $("#night_button").css("left", 10);  
   $("#night_button").css("top", 150);  

	$("#color_picker").append('<div class="popup_item" id="white_button" onClick="colorsAll(255,255,255,0)"> </div>');
	$("#white_button").css('background-color','rgb(255,255,255)');
  	$("#white_button").css("left", 50);  
  	$("#white_button").css("top", 150);
	
	$("#color_picker").append('<div class="popup_item" id="black_button" onClick="colorsAll(0,0,0,0)"> </div>');
	$("#black_button").css('background-color','rgb(0,0,0)');
	$("#black_button").css("left", 100);  
	$("#black_button").css("top", 150);
   
	   $("#timeline_save_button").css("right", $("#timeline_save_button").outerWidth());	
   $("#timeline_save_button").css("top", 0);	
   $("#timeline_save_button").css("background-image",'url(images/note_accept.png)');						
  $("#timeline_save_button").click(function(e){
									SaveCurrentTimeLine();
							});
							
							
   $("#timeline_add_button").css("right", 0);	
   $("#timeline_add_button").css("top", 0);	
   $("#timeline_add_button").css("background-image",'url(images/table_add.png)');						
  $("#timeline_add_button").click(function(e){
									AddTimeSlot("append");
							});
							
							
	
	$("#playlist").append('<ul id="saved_timeline_list"> </ul>');
	
	$( "#saved_timeline_list" ).selectable({
			stop: function() {
				selected_timelines.length = 0; //Empty array
				$("#log").html("");
				$( ".ui-selected", this ).each(function() {
					var name = $(this).text();
					$("#log").append(name + " ");
					selected_timelines.push( name );
				});
			}
		});
	
	$("#timeline_open_button").css("right", 0);	
	$("#timeline_open_button").css("top", 220);	
   $("#timeline_open_button").css("background-image",'url(images/note_edit.png)');						
  $("#timeline_open_button").click(function(e){
  									var file_to_open = selected_timelines[0];
									OpenExistingTimeline(file_to_open);
							});
	$("#timeline_delete_button").css("right", $("#timeline_delete_button").outerWidth()+5);	
	$("#timeline_delete_button").css("top", 220);	
   $("#timeline_delete_button").css("background-image",'url(images/trash_can_delete.png)');						
  $("#timeline_delete_button").click(function(e){
  									var file_to_delete= selected_timelines[0];
									DeleteExistingTimeline(file_to_delete);
							});
	$("#timeline_new_button").css("right", $("#timeline_new_button").outerWidth()*2 + 10);	
	$("#timeline_new_button").css("top", 220);	
   $("#timeline_new_button").css("background-image",'url(images/note_add.png)');						
  $("#timeline_new_button").click(function(e){
									OpenExistingTimeline("New");
									
							});
	
	loaddControlPanel();
}

function addTimelineToPlaylist(playlist,timelines_to_add){
	
	//$("#log").html( current_playlist + 'getting ');
	for(i in timelines_to_add){
		var name = timelines_to_add[i];
	//	$("#log").append(name+" ");
		$("#playlist_"+playlist+" ul").append('<li title="'+name+'" class="ui-state-default">'+name+' <span class="ui-icon ui-icon-circle-close">remove</span></li>');
		$("#playlist_"+playlist+" ul li span").click(function(e){
			var $target = $(e.target);
			$("#log").html("Click delete button for "+ $target.parent().id);
			$target.parent().remove();
		});
	}

}

function loaddControlPanel(){
   
   // $("#control_panel").append('<div id="schedule_button" class="popup_item"></div>');
    // $("#add_tab").css("right", 150); 
    //  $("#add_tab").css("top", 20);
   
   // $("#add_tab").css("float", "right");   
    
    $("#playlist_add").css("right", 150);  
    $("#playlist_add").css("top", 70);
	$("#playlist_add").button();
    $("#playlist_add").click(function() {
    					addTimelineToPlaylist(current_playlist,selected_timelines);
    				   });
   

   // $("#playlist_save").css("right", 150);  
   //     $("#playlist_save").css("top", 150);
	$("#playlist_save").button();
    $("#playlist_save").click(function() {
    					savePlaylist(current_playlist);
    				   });
   
    
 
  
}




/**
 * Converts an HSL color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
 * Assumes h, s, and l are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   Number  h       The hue
 * @param   Number  s       The saturation
 * @param   Number  l       The lightness
 * @return  Array           The RGB representation
 */
function hslToRgb(h, s, l){
    var r, g, b;

    if(s == 0){
        r = g = b = l; // achromatic
    }else{
        function hue2rgb(p, q, t){
            if(t < 0) t += 1;
            if(t > 1) t -= 1;
            if(t < 1/6) return p + (q - p) * 6 * t;
            if(t < 1/2) return q;
            if(t < 2/3) return p + (q - p) * (2/3 - t) * 6;
            return p;
        }

        var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        var p = 2 * l - q;
        r = hue2rgb(p, q, h + 1/3);
        g = hue2rgb(p, q, h);
        b = hue2rgb(p, q, h - 1/3);
    }

    return [r * 255, g * 255, b * 255];
}

/**
 * Converts an HSV color value to RGB. Conversion formula
 * adapted from http://en.wikipedia.org/wiki/HSV_color_space.
 * Assumes h, s, and v are contained in the set [0, 1] and
 * returns r, g, and b in the set [0, 255].
 *
 * @param   Number  h       The hue
 * @param   Number  s       The saturation
 * @param   Number  v       The value
 * @return  Array           The RGB representation
 */
function hsvToRgb(h, s, v){
    var r, g, b;

    var i = Math.floor(h * 6);
    var f = h * 6 - i;
    var p = v * (1 - s);
    var q = v * (1 - f * s);
    var t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return [r * 255, g * 255, b * 255];
}


function genColorTable(table_num, color_model){
	var num_brightness = 9;
	var num_colors = 15;
	$("#color_picker").append('<table id="color_table'+table_num+'" cellspacing="1" cellpadding="0" >');
	for(saturation = 50; saturation >= 0; saturation -=(50/num_brightness)){
		$("#color_table"+table_num).append("<tr>");
		for( hue = 0; hue <360; hue += (360/num_colors)){
			var rgb = new Array(3);
			if(color_model == "hsv"){
				rgb = hsvToRgb(hue / 360 ,saturation / 100,1);
			}else {
				rgb = hslToRgb(hue / 360 ,1,saturation / 100);
			}
			rgb[0]= Math.floor(rgb[0]);
			rgb[1]= Math.floor(rgb[1]);
			rgb[2]= Math.floor(rgb[2]);
			var rgb_string = "rgb("+rgb[0]+","+rgb[1]+","+rgb[2]+")";
			
			$("#color_table"+table_num+" tr").last().append('<td class="colorchart" style="background-color:'+rgb_string+'" onClick="colorsAll('+rgb[0]+','+rgb[1]+','+rgb[2]+',0)"></td>');
		}
		$("#color_table"+table_num).append("</tr>");
	}
}




