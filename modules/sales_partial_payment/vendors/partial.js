
var loadSelect2 = {
	'#stock_id, #customer_id, #supplier_id': function(e) {
		$(e).select2({
			dropdownAutoWidth : true,
			templateResult: function(item) { // break a select option item into multi lines
				var selectionText = item.text.split('\n');
				var returnString = $('<span></span>');
				$.each(selectionText, function(index, value){
					line = value === undefined ? '' : value;
					returnString.append(line + '</br>');
				})
						
				return returnString;
			}
		});
		$(e).on('select2:close', function() {
			$(this).focus();
		});
	}
}

var editableCells = {
	'input': function(e) {
		if(e.onfocus==undefined) {
			e.onfocus = function() {
				save_focus(this);
				if (string_contains(this.className, 'combo') || string_contains(this.className, 'combo3'))
					this.select();
			};
		}
		if (string_contains(e.className, 'combo') || string_contains(e.className, 'combo2') || string_contains(e.className, 'combo3')) {
				_set_combo_input(e);
		}
		else
    		if(e.type == 'text' ) {
   	  			e.onkeydown = function(ev) {
  					ev = ev||window.event;
  					key = ev.keyCode||ev.which;
 	  				if(key == 13) {
						if(e.className == 'amountbox' || e.className == 'searchbox')
							e.onblur();
						return false;
					}
					return true;
	  			}
			}
	},
	'div#items_table .amountbox': function(e) {
		$(e).focus(
			function(){
				$(this).select();
				$(this).css('outline','none');
				$(this).parent('td').css('background-color', '#FFFFFF');
			}
		);
		e.setAttribute('_last_val', e.value);
		e.setAttribute('autocomplete', 'off'); //must be off when calling onblur
  		e.onblur = function() {
  			$(this).parent('td').css('background-color', '');
  			var dec = this.getAttribute("dec");
			var val = this.getAttribute('_last_val');
			if (val != get_amount(this.name)) {
				this.setAttribute('_last_val', get_amount(this.name));
				price_format(this.name, get_amount(this.name), dec);
				JsHttpRequest.request('_'+this.name+'_changed', this.form);
			}
		}
	},
	'input[name=this_payment]': function(e) {
		$(e).blur(function() {
			$("button[name='update']").trigger('click');
		});
		$(e).keyup(function(k){
			if(k.keyCode == 13)
				$(this).blur();
		});
	}
}

Behaviour.register(loadSelect2);
Behaviour.register(editableCells);

