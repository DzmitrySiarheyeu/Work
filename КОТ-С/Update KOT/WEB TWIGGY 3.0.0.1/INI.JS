	function INIFile(text) {
        function GetINI(text) {
            var INI = {};
            var arr_lines = text.split('\n');
            var section, param, value;
            for (var i in arr_lines) {
                if (/^\[(\w+)\]/.test(arr_lines[i])) {
                    section = RegExp.$1;
                    INI[section] = {};
                }
                if (/^([^;#][^=]*?)\s*=\s*([^\r\n]*?)\s*$/.test(arr_lines[i])) {
                    param = RegExp.$1;
                    value = RegExp.$2;
                    INI[section][param] = value;
                }
            }

            return INI;
        }

        var _ini = GetINI(text);

        this.GetValue = function (top, elem) {
            if (top in _ini)
                if (elem in _ini[top])
                    return _ini[top][elem];
            return null;
        }

        this.SetValue = function (top, elem, value) {
            if (top in _ini) {
                _ini[top][elem] = value;
            } else {
                _ini[top] = {};
                _ini[top][elem] = value;
            }
        }
		
	this.GetINIFile = function () {
            var file = String();
            var top, elem;
            for(top in _ini) {
                file += "[" + top + "]\r\n"; 
		for(elem in _ini[top]) {
		file += elem + "=" + _ini[top][elem] + "\r\n";
		}
            }		
            return file;
	}

    this.GetAllTopValues = function (top) {
        if (top in _ini)
                return _ini[top];
            return null;
        }
        
        this.GetTops = function(regExp) {
            var tops = [];
            if(regExp == null)
                regExp = /.+/;
            for(var top in _ini) {
                if(top.match(regExp))
                    tops[tops.length] = top;
            }
            return tops;
        }

        this.DeleteTop = function(top) {
            if(top in _ini)
                delete _ini[top];
        }
		
		this.DeleteTopByTempalate = function(template) {
			for(var top in _ini){
				if(template.test(top))
					delete _ini[top];
			}
		}
    }

