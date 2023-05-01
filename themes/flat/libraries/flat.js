$(document).ready(function() {

	switch (user.datefmt) {
		case 0: datefmt = 'mm'+user.datesep+'dd'+user.datesep+'yy'; break;
		case 1: datefmt = 'dd'+user.datesep+'mm'+user.datesep+'yy'; break;
		case 2: datefmt = 'yy'+user.datesep+'mm'+user.datesep+'dd'; break;
		case 3: datefmt = 'MM'+user.datesep+'dd'+user.datesep+'yy'; break;
		case 4: datefmt = 'dd'+user.datesep+'MM'+user.datesep+'yy'; break;
		case 5: datefmt = 'yy'+user.datesep+'MM'+user.datesep+'dd'; break;
		default: datefmt = 'dd'+user.datesep+'mm'+user.datesep+'yy';
	}

	$('select:not([multiple])').select2({dropdownAutoWidth : true});
	$('select').on('select2:close', function() { $(this).focus(); });

	$('select').on('select2:open', function(e){
		if($('.select2-dropdown > div').length > 0)
			$('.select2-dropdown').find('div').remove();

		var onclick_val = $(e.target).parent().siblings('img').attr('onclick');
		if(typeof onclick_val != 'undefined') {
			$('.select2-dropdown').append('<div><i class="fa fa-search"></i>Advanced Search</div>');
			$('.select2-dropdown > div').attr('onclick', onclick_val).click(function(){
				$('select').select2('close');
			});
		}
		$.each(editors, function(key, val) {
			if($(e.target).attr('id') == val[1]) {
				$('.select2-dropdown').append('<div key="'+key+'"><i class="fa fa-plus"></i>Add new (F' + (key-111) + ')</div>');
				$('.select2-dropdown > div').attr('key', key).click(function(){
					var e2 = jQuery.Event('keydown');
                    e2.keyCode = key;                     
                    $('.select2-dropdown').trigger(e2);
					$('select').select2('close');
				});
			}
		});
	});
				
	$(document).tooltip().off('focusin focusout');
	$('.ajaxsubmit, input[type=checkbox], .editbutton').tooltip().click(function() {
		$(this).tooltip('close');
	})

    $('.date').datepicker({
    	onSelect:function(){
    		$(this).attr('_last_val', this.value);
    		$('input[name=search]').trigger('click');
    	},
		dateFormat: datefmt,
        changeMonth: true,
        changeYear: true,
		showWeek: true,
      	firstDay: 1,
		showOn: 'button',
      	buttonImage: '../themes/flat/images/calendar_grey.svg',
      	buttonImageOnly: true,
      	buttonText: 'Select date'
    });
			
	$('#menu_toggle').click(function() {
    	$('.right-body').toggleClass('full_width').toggleClass('has_side_bar');
		$('.main-menu').toggleClass('deactive_sidebar').toggleClass('active_sidebar');
		$('.frontBar').toggleClass('frontBar_full_width');
		$('#footer .footer').toggleClass('footer_full_width');
		if($(window).width() < 481)
			$('html, body').animate({scrollTop: '0px'}, 0);
	});
	$('#module_panel_open, #module_panel_close').click(function() {
    	$('.frontBar').toggleClass('frontBar_full_screen');
		$('.right-body').toggleClass('right_body_hide');
	});
	if(localStorage.getItem('dark') == 'true') {
		$('.main-menu').addClass('dark-menu');
		$('.frontBar').addClass('dark-frontBar');
	}
	$('#night-mode').click(function() {
		$('.main-menu').toggleClass('dark-menu');
		$('.frontBar').toggleClass('dark-frontBar');
		localStorage.setItem('dark', $('.main-menu').hasClass('dark-menu'));
	});
	$('a[data-toggle="tab"]').on('shown.bs.tab', function (e) {
		$(e.currentTarget.hash).find('.ct-chart').each(function(el, tab) {
			tab.__chartist__.update();
		});
	});
});