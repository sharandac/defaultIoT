var connect = false;
var timeout = 3;
var savecounter = 0;
var connection;
var configname = "";
var statusname = "";
var savename = "";

getconnect();
setInterval(function () {getStatus();}, 1000);

/**
 * @brief set function name to get settings via websockets
 * @param {*} namespace 
 */
function set_settings_namespace( namespace ) {
    configname = namespace;
}
/**
 * @brief set function name to get status via websockets
 * @param {*} namespace 
 */
function set_status_namespace( namespace ) {
    statusname = namespace;
}
/**
 * @brief set function name to save config via websockets
 * @param {*} namespace 
 */
function set_save_namespace( namespace ) {
    savename = namespace;
}

function getconnect() {
    connection = new WebSocket('ws://' + location.hostname + '/ws', ['arduino']);

    connection.onopen = function () {
        connect = true; 	
        refreshValue();
        get_settings();
    };

    connection.onmessage = function (e) {
        console.log('Server: ', e.data);
        partsarry = e.data.split('\\');
        /**
         * status\Save
         */
        if( partsarry[0] == 'status' ) {
            if ( partsarry[1] == 'Save' ) {
                document.getElementById('fixedfooter').style.background = "#008000";
                savecounter = 2;
            }
            else {
                document.getElementById('status').firstChild.nodeValue = partsarry[1];
            }
    
            timeout = 4;
            return;
        }
        /**
         * option\selectid\value
         */
        if( partsarry[0] == 'option') {
            if ( document.getElementById( partsarry[1] ) ) {
                document.getElementById( partsarry[1] ).value = partsarry[2];
            }
            return;
        }
        /**
         * label\labelid\value
         */
        if( partsarry[0] == 'label') {
            if ( document.getElementById( partsarry[1] ) ) {
                document.getElementById( partsarry[1] ).textContent = partsarry[2];
            }
            return;
        }
        /**
         * checkbox\checkboxid\true/false
         */
        if( partsarry[0] == 'checkbox') {
            if ( document.getElementById( partsarry[1] ) ) {
                if( partsarry[2] == 'true' )
                    document.getElementById( partsarry[1] ).checked = true;
                else
                    document.getElementById( partsarry[1] ).checked = false;
            }
            return;
        }
        /**
         * id\value
         */
        if ( document.getElementById( partsarry[0] ) ) {
            document.getElementById( partsarry[0] ).value = partsarry[1];
        }
    }

    connection.onclose = function(e) {
        console.log('Socket is closed. Reconnect will be attempted in 1 second.', e.reason);
        connect = false;
        setTimeout(function() { getconnect(); }, 1000);
    };

    connection.onerror = function (error) {
        console.log('WebSocket Error ', error);
        connect = false; 
        connection.close();
    };
}

function sendCMD( value ) {
    console.log( "Client: " + value );
	if ( connect )
        connection.send( value );
    else
        console.log( "no connection" );
}

function refreshValue() {
    sendCMD("STA");
}

function SaveSettings() {
    if( savename != "" )
        sendCMD( savename );
    else
        sendCMD("SAV");
}

function get_settings() {
    sendCMD( configname );
}

function SendCheckboxSetting( value ) {
    if( document.getElementById( value ) ) {
        if( document.getElementById( value ).checked )
            sendCMD( value + "\\" + 1 );
        else
            sendCMD( value + "\\" + 0 );
    }
}

function SendSetting( value ) {
    sendCMD( value + "\\" + document.getElementById( value ).value );
}

function getStatus() {
    if( statusname != "" )
        sendCMD( statusname );

    sendCMD( "STS" );

    savecounter--;
    timeout--;

    if ( timeout > 0 && savecounter < 0 ) {
        document.getElementById('fixedfooter').style.background = "#333333";
    }
    else if ( timeout < 0 ) {
            if ( document.getElementById( 'status' ) ) { 
                document.getElementById('fixedfooter').style.background = "#800000";
                document.getElementById( 'status' ).firstChild.nodeValue = 'offline ... wait for reconnect';
            }
    }
}


