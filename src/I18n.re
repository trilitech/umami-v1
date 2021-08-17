/*****************************************************************************/
/*                                                                           */
/* Open Source License                                                       */
/* Copyright (c) 2019-2021 Nomadic Labs, <contact@nomadic-labs.com>          */
/*                                                                           */
/* Permission is hereby granted, free of charge, to any person obtaining a   */
/* copy of this software and associated documentation files (the "Software"),*/
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense,  */
/* and/or sell copies of the Software, and to permit persons to whom the     */
/* Software is furnished to do so, subject to the following conditions:      */
/*                                                                           */
/* The above copyright notice and this permission notice shall be included   */
/* in all copies or substantial portions of the Software.                    */
/*                                                                           */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR*/
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL   */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER*/
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING   */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER       */
/* DEALINGS IN THE SOFTWARE.                                                 */
/*                                                                           */
/*****************************************************************************/

let p = Format.sprintf;

let btn = {
  pub create_or_import_account = "CREATE OR IMPORT ACCOUNT";
  pub _this = this;
  pub customize_derivation_path = "Customize derivation path";
  pub ok = "OK";
  pub cancel = "CANCEL";
  pub back = "BACK";
  pub continue = "CONTINUE";
  pub delete = "DELETE";
  pub create = "CREATE";
  pub finish = "FINISH";
  pub add = "ADD";
  pub send = "SEND";
  pub save = "SAVE";
  pub done_ = "SIMPLE VIEW";
  pub edit = "MANAGEMENT VIEW";
  pub validate_save = "VALIDATE AND SAVE";
  pub go_back = "GO BACK";
  pub go_operations = "GO TO OPERATIONS";
  pub confirm = "CONFIRM";
  pub update = "UPDATE";
  pub register = "REGISTER";
  pub logs = "LOGS";
  pub import_account = "IMPORT EXISTING ACCOUNT";
  pub create_account_new = "CREATE NEW ACCOUNT";
  pub create_account_record_ok = {js|OK, I’VE RECORDED IT|js};
  pub add_account = "ADD ACCOUNT";
  pub add_contact = "ADD CONTACT";
  pub add_token = "REGISTER TOKEN";
  pub advanced_options = "Advanced options";
  pub delegate = "DELEGATE";
  pub send_another_transaction = "ADD ANOTHER TRANSACTION";
  pub start_batch_transaction = "START BATCH TRANSACTION";
  pub add_transaction = "ADD TRANSACTION";
  pub send_submit = "SUBMIT TRANSACTION";
  pub batch_submit = "SUBMIT BATCH";
  pub send_cancel = "CANCEL TRANSACTION";
  pub delegation_submit = "CONFIRM DELEGATION";
  pub delegated = "DELEGATED";
  pub delete_token = "Delete token";
  pub import = "IMPORT";
  pub export = "EXPORT";
  pub scan = "SCAN";
  pub upgrade = "Download";
  pub load_file = "LOAD FROM FILE";
  pub csv_format_link = "CSV specification and example";
  pub disclaimer_agree = "CONTINUE";
  pub deny = "DENY";
  pub allow = "ALLOW";
  pub reject = "REJECT";
  pub sign = "SIGN";
  pub close = "CLOSE";
  pub add_custom_network = "ADD CUSTOM NETWORK";
  pub beacon_connect_dapp = "CONNECT TO DAPP";
  pub custom = "CUSTOM";
  pub see_list = "SEE LIST"
};

let tooltip = {
  pub close = "Close";
  pub _this = this;
  pub show_qr = "Show QR Code";
  pub copy_clipboard = "Copy to clipboard";
  pub addressbook_edit = "Edit contact";
  pub addressbook_delete = "Delete contact";
  pub add_contact = "Add to contacts";
  pub no_tez_no_delegation = "Delegation requires tez";
  pub refresh = "Refresh";
  pub open_in_explorer = "Open in explorer";
  pub update_delegation = "update delegation";
  pub chain_not_connected = "Not connected to a chain";
  pub custom_network_edit = "Edit network";
  pub custom_network_delete = "Delete network"
};

let log = {
  pub _this = this;
  pub copied_to_clipboard = p("%s copied to clipboard");
  pub log_content = "Log content";
  pub address = "Address";
  pub beacon_sign_payload = "Sign payload"
};

let label = {
  pub _this = this;
  pub recovery_phrase_format = "Recovery phrase format";
  pub account_create_name = "Name";
  pub add_contact_name = "Name";
  pub add_contact_address = "Address";
  pub send_amount = "Amount";
  pub send_sender = "Sender account";
  pub send_recipient = "Recipient account";
  pub password = "Password";
  pub fee = "Fee";
  pub implicit_reveal_fee = "Implicit reveal fee";
  pub gas_limit = "Gas limit";
  pub storage_limit = "Storage limit";
  pub counter = "Counter";
  pub force_low_fee = "Force low fee";
  pub confirm_password = "Confirm password";
  pub account_delegate = "Account to delegate";
  pub baker = "Baker";
  pub add_token_address = "Address";
  pub add_token_name = "Name";
  pub add_token_symbol = "Symbol";
  pub summary_subtotal = "Subtotal";
  pub summary_total = "Total";
  pub summary_total_tez = "Total tez";
  pub transactions = "Transactions";
  pub account_secret = "Root";
  pub account_umami = "Umami";
  pub token = "Token";
  pub account_cli = "Cli";
  pub account_default_path = "Default Path - m/44'/1729'/?'/0'";
  pub account_custom_path = "Custom Path";
  pub beacon_account = "Account to connect to dApp";
  pub beacon_sign_payload = "Payload to sign";
  pub beacon_dapp_pairing = "DApp pairing request";
  pub parameters = "Parameters";
  pub custom_network_name = "Name";
  pub custom_network_mezos_url = "Mezos URL";
  pub custom_network_node_url = "Node URL"
};

let input_placeholder = {
  pub _this = this;
  pub tez_amount = "0.000000";
  pub token_amount = "0";
  pub add_accounts_name = "Enter account's name";
  pub add_contacts_name = "Enter contact's name";
  pub add_contacts_tz = "Enter contact's tz address";
  pub add_contact_or_tz = "Paste a tz address or type in a contact's name";
  pub add_token_address = "Enter KT1 address of a contract";
  pub add_token_name = "e.g. Tezos";
  pub enter_new_password = "Enter new password, at least 8 characters";
  pub confirm_password = "Confirm your new password";
  pub enter_password = "Enter your password";
  pub add_token_symbol = "e.g. tez, KLD, ...";
  pub enter_derivation_path = "Enter your derivation path";
  pub custom_network_name = "e.g. Test Network";
  pub custom_network_node_url = "e.g. https://testnet-tezos.giganode.io/";
  pub custom_network_mezos_url = "e.g. https://api.umamiwallet.com/florencenet"
};

let form_input_error = {
  pub _this = this;
  pub dp_not_a_dp = "Not a derivation path";
  pub dp_more_than_1_wildcard = "Cannot have more that one '?'";
  pub dp_missing_wildcard = "Missing '?' or 0";
  pub dp_not_tezos = "Not a BIP44 Tezos Path";
  pub empty_transaction = "Transaction is empty.";
  pub branch_refused_error = "Please retry. An error came up while communicating with the node";
  pub key_already_registered = a =>
    p("Address already registered under: %s", a);
  pub name_already_registered = "Name already registered";
  pub mandatory = "This input is mandatory";
  pub int = "must be an integer";
  pub float = "must be a float";
  pub change_baker = "It must be a different baker than the current one";
  pub wrong_password = "Wrong password";
  pub password_length = "Password length must be at least 8 characters";
  pub unregistered_delegate = "This key is not registered as a baker";
  pub bad_pkh = "Not a valid key";
  pub invalid_contract = "The recipient is not a key or an alias";
  pub confirm_password = "It must be the same password";
  pub derivation_path_error = "Invalid derivation path";
  pub name_already_taken = a => p("%s is already taken", a);
  pub api_not_available = "API not available";
  pub node_not_available = "Node not available";
  pub api_and_node_not_available = "API and node both not available";
  pub different_chains = "API and Node are not running on the same network"
};

let title = {
  pub _this = this;
  pub error_logs = "Logs";
  pub account_create = "Create new account";
  pub derive_account = "Add account";
  pub account_update = "Edit account";
  pub import_account = "Import account";
  pub secret_update = "Edit secret";
  pub add_contact = "Add contact";
  pub update_contact = "Edit contact";
  pub add_token = "Register Token";
  pub export = "Export";
  pub account_create_password = "Set a password to secure your wallet";
  pub account_enter_password = "Please enter password to confirm";
  pub account_create_verify_phrase = "Verify your recovery phrase";
  pub account_derivation_path = "Derivation path";
  pub import_account_enter_phrase = "Enter your recovery phrase";
  pub operation_submitted = "Operation Submitted";
  pub sender_account = "Sender account";
  pub recipient_account = "Recipient account";
  pub delegated_account = "Account to delegate";
  pub baker_account = "Baker";
  pub withdraw_baker = "Withdraw baker";
  pub confirm_delegate = "Confirm delegate";
  pub delegate = "Delegate";
  pub delegate_update = "Change baker";
  pub delegate_delete = "End delegation";
  pub delegation_sent = "Delegation sent";
  pub baker_updated = "Baker updated";
  pub delegation_deleted = "Delegation deleted";
  pub delete_contact = "Delete contact?";
  pub deleted_contact = "Contact deleted";
  pub delete_token = "Delete token";
  pub deleted_token = "Token deleted";
  pub delete_beacon_peer = "Delete peer";
  pub delete_beacon_permission = "Delete permission";
  pub confirm_cancel = "Are you sure you want to cancel?";
  pub send = "Send";
  pub confirmation = "Confirmation";
  pub simulation = "Simulation";
  pub creating_account = "Creating account";
  pub importing_account = "Importing account";
  pub submitting = "Submitting operation";
  pub send_many_transactions = "Send one or many transactions";
  pub delete_account = "Delete account?";
  pub delete_secret = "Delete secret?";
  pub delete_load = "Deleting";
  pub delete_account_done = "Account deleted";
  pub batch = "Batch";
  pub scan = "Scan";
  pub disclaimer = "User Agreement";
  pub beacon_connection_request = "Connection Request";
  pub beacon_sign_request = "Sign";
  pub beacon_pairing = "Connect to DApp with pairing request";
  pub beacon_pairing_qr = "Connect to DApp with QR code";
  pub beacon_scan_qr = "Scan QR code from dApp";
  pub interaction = "Interaction";
  pub add_custom_network = "Add Custom Network";
  pub update_custom_network = "Edit Custom Network";
  pub delete_custom_network = "Delete Network?";
  pub beacon_error = "Beacon Error"
};

let expl = {
  pub _this = this;
  pub send_many_transactions = "You have the ability to submit a batch of transactions, but please note that the batch will be confirmed as a whole: should one transaction in the batch fail then the whole batch will not be completed.";
  pub account_create_record_recovery = {j|Please record the following 24 words in sequence in order to restore it in the future. Ensure to back it up, keeping it securely offline.|j};
  pub account_create_record_verify = {j|We will now verify that you’ve properly recorded your recovery phrase. To demonstrate this, please type in the word that corresponds to each sequence number.|j};
  pub account_create_password_not_recorded = {j|Please note that this password is not recorded anywhere and only applies to this machine.|j};
  pub account_select_derivation_path = {j|Umami wallet supports custom derivation path to select new addresses. You may also select the default derivation path and use the default key.|j};
  pub import_account_enter_phrase = {j|Please fill in the recovery phrase in sequence.|j};
  pub confirm_operation = "Please validate the details of the transaction and enter password to confirm";
  pub batch = "Review, edit or delete the transactions of the batch";
  pub operation = "The operation will be processed and confirmed, you can see its progress in the Operations section.";
  pub beacon_dapp = "would like to connect to your wallet";
  pub beacon_dapp_request = {js|This site is requesting access to view your account’s address.\nAlways make sure you trust the sites you interact with.|js};
  pub beacon_operation = "Requests Operations";
  pub beacon_delegation = "Request Delegation";
  pub beacon_dapp_sign = "requests your signature";
  pub custom_network = "Please specify the parameters of the network you want to connect to."
};

let menu = {
  pub _this = this;
  pub app_menu_support = "Support";
  pub app_menu_help = "Help";
  pub app_menu_new_version = "Check for new versions";
  pub app_menu_website = "Website";
  pub operation_register_as_baker = "Register as new baker";
  pub operation_delegate_to = p("Delegate_to %s");
  pub operation_unknown = "Unknown";
  pub operation_cancel_delegation = "Cancel delegation";
  pub delegate_edit = "Change baker";
  pub delegate_delete = "End delegation";
  pub batch_edit = "Edit transfer";
  pub batch_delete = "Delete transfer";
  pub delete_account = "Delete";
  pub delete_secret = "Delete";
  pub edit = "Edit"
};

let settings = {
  pub _this = this;
  pub theme_title = "THEME";
  pub theme_system = "Default to system";
  pub theme_dark = "Dark";
  pub theme_light = "Light";
  pub confirmations_title = "VERIFICATION";
  pub confirmations_label = "Number of confirmations (blocks)";
  pub confirmations_saved = "Number of confirmations Saved";
  pub chain_title = {j|CHAIN/NETWORK|j};
  pub chain_node_label = "Node URL";
  pub chain_mezos_label = "Mezos URL";
  pub chain_saved = "Chain URLs Saved";
  pub token_title = "TOKENS";
  pub token_label = "Token Balance Viewer Contract";
  pub token_saved = "Token Balance Viewer Contract Saved";
  pub danger_title = "DANGER ZONE";
  pub danger_reset_section = "Reset settings";
  pub danger_reset_text = {js|This will remove or restore custom settings to default values.|js};
  pub danger_reset_button = "RESET";
  pub danger_reset_confirm_title = "Reset settings";
  pub danger_reset_confirm_text = {js|This will remove or reset all customized settings to their defaults. Personnal data -including saved contacts, password and accounts- won't be affected.|js};
  pub danger_reset_confirm_button = "RESET SETTINGS";
  pub danger_offboard_section = "Offboard wallet";
  pub danger_offboard_text = {js|Offboarding will permanently delete any data from this computer. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the backup phrase.|js};
  pub danger_offboard_button = "OFFBOARD";
  pub danger_offboard_form_title = "Offboard wallet";
  pub danger_offboard_form_text = {js|Offboarding will permanently delete any data from this computer. Please acknowledge that you have read and understood the disclaimer, then enter « wasabi » to confirm. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the recovery phrase.|js};
  pub danger_offboard_form_checkbox_label = {js|I have read the warning and I am certain I want to delete my private keys locally. I also made sure to keep my recovery phrase.|js};
  pub danger_offboard_form_input_placeholder = "Enter code word to confirm";
  pub danger_offboard_form_input_error = "Not the correct confirm code word";
  pub beacon_title = "DAPPS";
  pub beacon_peers_section = "Peers";
  pub beacon_peers_name = "NAME";
  pub beacon_peers_relay = "RELAY SERVER";
  pub beacon_empty_peers = "No peer";
  pub beacon_permissions_section = "Permissions";
  pub beacon_permissions_dapp = "DAPP";
  pub beacon_permissions_account = "CONNECTED ACCOUNT";
  pub beacon_permissions_scopes = "SCOPES";
  pub beacon_permissions_network = "NETWORK";
  pub beacon_empty_permissions = "No permission"
};

let network = {
  pub _this = this;
  pub api_not_available = "The API is not available";
  pub api_version_rpc_error = a =>
    p("The API `/version` RPC parsing returned an error: %s", a);
  pub api_version_format_error = a =>
    p(
      "The API `/version` RPC parsing returned an unknown version format: %s",
      a,
    );
  pub api_monitor_rpc_error = a =>
    p("The API `/monitor/blocks` RPC parsing returned an error: %s", a);
  pub node_not_available = "The node is not available";
  pub node_version_rpc_error = a =>
    p("Node `/chains/main/chain_id` RPC parsing returned an error: %s", a);
  pub chain_inconsistency = (a, b) =>
    p(
      "The API and Node are not running on the same network.\n\
     API runs on chain `%s` and the node on the chain `%s`.",
      a,
      b,
    );
  pub unknown_chain_id = a =>
    p(
      "Network %s is not supported, your operation cannot be viewed in an explorer.",
      a,
    );
  pub api_not_supported = a =>
    p("The API %s is not supported by this version of Umami.", a);
  pub api_and_node_not_available = "API and Node are both not available"
};

let taquito = {
  pub _this = this;
  pub not_an_account = "Not a tz address";
  pub not_a_contract = "Not a contract address";
  pub no_prefix_matched = "Unknown address prefix";
  pub invalid_checksum = "Invalid checksum";
  pub invalid_length = "Invalid length";
  pub valid = "Valid";
  pub unknown_error_code = n => p("Unknown error code %d", n)
};

let wallet = {pub _this = this; pub key_not_found = "Key not found"};

let errors = {
  pub _this = this;
  pub no_secret_found = "No secrets found";
  pub secret_not_found = i => p("Secret at index %d not found!", i);
  pub cannot_update_secret = i => p("Can't update secret at index %d!", i);
  pub recovery_phrase_not_found = i =>
    p("Recovery phrase at index %d not found!", i);
  pub beacon_transaction_not_supported = "Beacon transaction not supported";
  pub beacon_request_network_missmatch = "Beacon request network not supported";
  pub video_stream_access_denied = "Unable to access video stream\n(please make sure you have a webcam enabled)";
  pub incorrect_number_of_words = "Mnemonic must have 12, 15 or 24 words.";
  pub unknown_bip39_word = (w, i) =>
    p(
      "Word %d ('%s') is not a valid BIP39 word, please refer to the standard.",
      i,
      w,
    );
  pub secret_already_imported = "Secret already imported"
};

let csv = {
  pub _this = this;
  pub cannot_parse_number = (row, col) =>
    p("Value at row %d column %d is not a number", row, col);
  pub cannot_parse_boolean = (row, col) =>
    p("Value at row %d column %d is not a boolean", row, col);
  pub cannot_parse_custom_value = (err, row, col) =>
    p("Value at row %d column %d is not valid:\n%s", row, col, err);
  pub cannot_parse_row = row =>
    p("Row %d is not valid, some columns are probably missing", row);
  pub cannot_parse_csv = p("CSV is not valid");
  pub no_rows = p("CSV is empty");
  pub cannot_mix_tokens = row =>
    p(
      "Tokens from CSV must be all the same. Row %d is different from the previous ones.",
      row,
    );
  pub cannot_parse_token_amount = (v, row, col) =>
    p(
      "Value %s at row %d column %d is not a valid token amount",
      ReBigNumber.toString(v),
      row,
      col,
    );
  pub cannot_parse_tez_amount = (v, row, col) =>
    p(
      "Value %s at row %d column %d is not a valid tez amount",
      ReBigNumber.toString(v),
      row,
      col,
    );
  pub unknown_token = p("Unknown token %s");
  pub cannot_parse_address = (a, reason) =>
    p("%s in not a valid address: %s.", a, reason);
  pub cannot_parse_contract = (a, reason) =>
    p("%s in not a valid contract address: %s.", a, reason)
};

let disclaimer = {
  pub _this = this;
  pub last_updated = date => p("Last updated %s.", date);
  pub please_sign = "In order to use Umami, \nyou must agree to the terms.";
  pub agreement_checkbox = "Check here to indicate that you have read and \
     agree to the terms of the User Agreement"
};

let t = {
  pub unknown_operation = "Unknown";
  pub error404 = "404 - Route Not Found :(";
  pub logs_no_recent = "No recent messages";
  pub logs_clearall = "CLEAR ALL";
  pub amount = (a, b) => p("%s %s", a, b);
  pub tezos = "Tez";
  pub tez = "tez";
  pub mainnet = "Mainnet";
  pub florencenet = "Florencenet";
  pub granadanet = "Granadanet";
  pub testnet = "Testnet";
  pub add_custom_network = "ADD CUSTOM NETWORK";
  pub tez_amount = a => p("%s %s", a, this#tez);
  pub tez_op_amount = op => p("%s %a", op, () => this#tez_amount);
  pub address = "Address";
  pub account = "Account";
  pub operation_reveal = "Reveal";
  pub operation_transaction = "Transaction";
  pub operation_origination = "Origination";
  pub operation_delegation = "Delegation";
  pub operation_column_type = "TYPE";
  pub operation_column_amount = "AMOUNT";
  pub operation_column_recipient = "RECIPIENT";
  pub operation_column_timestamp = "TIMESTAMP";
  pub operation_column_fee = "FEE";
  pub operation_column_sender = "SENDER";
  pub operation_column_status = "STATUS";
  pub state_mempool = "Mempool";
  pub state_levels = p("%d/%d blocks");
  pub state_confirmed = "Confirmed";
  pub stepof = p("Step %d of %d");
  pub account_create_record_recovery = "Record your recovery phrase";
  pub contact_added = "Contact added";
  pub account_created = "Account created";
  pub account_updated = "Account updated";
  pub secret_updated = "Secret updated";
  pub token_contract = p("%s Token Contract");
  pub token_created = "Token created";
  pub operation_hash = "Operation Hash";
  pub account_imported = "Account imported";
  pub operation_summary_fee = p("+ Fee %a", () => this#tez_amount);
  pub navbar_accounts = "ACCOUNTS";
  pub navbar_operations = "OPERATIONS";
  pub navbar_addressbook = {j|ADDRESS BOOK|j};
  pub navbar_delegations = "DELEGATIONS";
  pub navbar_tokens = "TOKENS";
  pub navbar_settings = "SETTINGS";
  pub navbar_logs = "LOGS";
  pub delegate_column_account = "ACCOUNT";
  pub delegate_column_initial_balance = "INITIAL BALANCE";
  pub delegate_column_current_balance = "CURRENT BALANCE";
  pub delegate_column_duration = "DURATION";
  pub delegate_column_last_reward = "LAST REWARD";
  pub delegate_column_baker = "BAKER";
  pub token_column_name = "NAME";
  pub token_column_symbol = "SYMBOL";
  pub token_column_address = "ADDRESS";
  pub empty_token = "No token registered on the current chain";
  pub empty_delegations = "No delegation";
  pub empty_operations = "No operation";
  pub empty_address_book = "No contact";
  pub add_token_format_contract_sentence = {js|Please specify the address of a FA1.2 token contract for which you would like to view balances as well as to perform operations.|js};
  pub delegation_removal = "Delegation Removal";
  pub error_check_contract = "Address is not a valid token contract";
  pub words = p("%d words");
  pub upgrade_notice = "We recommend you upgrade your version of Umami.";
  pub custom_network_created = "Network created";
  pub custom_network_updated = "Network updated";
  pub custom_network_deleted = "Network deleted"
};
