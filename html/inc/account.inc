<?php
// This file is part of BOINC.
// http://boinc.berkeley.edu
// Copyright (C) 2017 University of California
//
// BOINC is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// BOINC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with BOINC.  If not, see <http://www.gnu.org/licenses/>.

// functions related to account creation and login:
// - forms for create / login
// - function to make login token

// make login token, store in user record, return token
//
function make_login_token($user) {
    $token = substr(random_string(), 0, 8);
    $now = time();
    $user->update("login_token='$token', login_token_time=$now");
    return $token;
}

// return HTML string for a checkbox for toggling password visibility
//
function passwd_visible_checkbox($name) {
    return sprintf('
<script>
function toggle_passwd() {
    var x = document.getElementById("%s");
    if (x.type === "password") {
        x.type = "text";
    } else {
        x.type = "password";
    }
}
</script>
<input type="checkbox" onclick="toggle_passwd()"><small> Show password</small>
        ', $name
    );
}

function create_account_form($teamid, $next_url) {
    global $recaptcha_public_key;
    form_input_hidden('next_url', $next_url);

    if ($teamid) {
        form_input_hidden('teamid', $teamid);
    }

    // Using invitation codes to restrict access?
    //
    if (defined('INVITE_CODES')) {
        form_input_text(
            sprintf('<span title="%s">%s</span>',
                tra("An invitation code is required to create an account."),
                tra("Invitation code")
            ),
            "invite_code"
        );
    }

    form_input_text(
        sprintf('<span title="%s">%s</span>',
            tra("Identifies you on our web site. Use your real name or a nickname."),
            tra("Screen name")
        ),
        "new_name"
    );
    form_input_text(
        sprintf('<span title="%s">%s</span>',
            tra("Must be a valid address of the form 'name@domain'."),
            tra("Email address")
        ),
        "new_email_addr"
    );
    $min_passwd_length = parse_element(get_config(), "<min_passwd_length>");
    if (!$min_passwd_length) {
        $min_passwd_length = 6;
    }

    form_input_text(
        sprintf('<span title="%s">%s</span>',
            tra("Must be at least %1 characters", $min_passwd_length),
            tra("Password")
        ),
        "passwd", "", "password"
    );
    form_input_text(tra("Confirm password"), "passwd2", "", "password");
    form_select(
        sprintf('<span title="%s">%s</span>',
            tra("Select the country you want to represent, if any."),
            tra("Country")
        ),
        "country",
        country_select_options()
    );
    if (POSTAL_CODE) {
        form_input_text(
            tra("Postal or ZIP Code")."<br><small>".tra("Optional")."</small>",
            "postal_code"
        );
    }
}

function login_form($next_url) {
    form_start(secure_url_base()."/login_action.php", "post");
    form_input_hidden("next_url", $next_url);
    if (LDAP_HOST) {
        $x = "Email address or LDAP user name:";
    } else {
        $x = tra("Email address:");
    }
    form_input_text($x, "email_addr");
    form_input_text(
        tra("Password:").'<br><small><a href="get_passwd.php">' . tra("forgot password?") . "</a></small>",
        "passwd",
        "",
        "password"
    );
    form_checkboxes(tra("Stay logged in"),
        array(array("stay_logged_in", "", true))
    );
    form_submit("Log in");
    form_end();
}

?>
