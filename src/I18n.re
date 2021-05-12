let p = Format.sprintf;

let btn = {
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
  pub load_file = "LOAD FROM FILE"
};

let tooltip = {
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
  pub chain_not_connected = "Not connected to a chain"
};

let log = {
  pub _this = this;
  pub copied_to_clipboard = p("%s copied to clipboard");
  pub log_content = "Log content";
  pub address = "Address"
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
  pub account_custom_path = "Custom Path"
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
  pub enter_derivation_path = "Enter your derivation path"
};

let form_input_error = {
  pub _this = this;
  pub branch_refused_error = "Please retry. An error came up while communicating with the node";
  pub key_already_registered = a =>
    p("Address already registered under: %s", a);
  pub name_already_registered = "Name already registered";
  pub string_empty = "This input is mandatory";
  pub int = "must be an integer";
  pub float = "must be a float";
  pub change_baker = "It must be a different baker than the current one";
  pub wrong_password = "Wrong password";
  pub password_length = "Password length must be at least 8 characters";
  pub unregistered_delegate = "This key is not registered as a baker";
  pub bad_pkh = "Not a valid key";
  pub invalid_contract = "The recipient is not a key or an alias";
  pub confirm_password = "It must be the same password";
  pub derivation_path_error = "Invalid derivation path"
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
  pub scan = "Scan"
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
  pub operation = "The operation will be processed and confirmed, you can see its progress in the Operations section."
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
  pub danger_offboard_form_input_error = "Not the correct confirm code word"
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
    p("The API %s is not supported by this version of Umami.", a)
};

let csv = {
  pub _this = this;
  pub cannot_parse_number = (row, col) =>
    p("Value at row %d column %d is not a number", row, col);
  pub cannot_parse_boolean = (row, col) =>
    p("Value at row %d column %d is not a boolean", row, col);
  pub cannot_parse_custom_value = (row, col) =>
    p("Value at row %d column %d is not valid", row, col);
  pub cannot_parse_row = row =>
    p("Row %d is not valid, some columns are probably missing", row);
  pub cannot_parse_csv = p("CSV is not valid");
  pub no_rows = p("CSV is empty");
  pub cannot_mix_tokens = p("Tokens from CSV must be all the same");
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
  pub unknown_token = "Unknown token"
};

let t = {
  pub error404 = "404 - Route Not Found :(";
  pub logs_no_recent = "No recent messages";
  pub logs_clearall = "CLEAR ALL";
  pub amount = (a, b) => p("%s %s", a, b);
  pub tezos = "Tez";
  pub xtz = "tez";
  pub mainnet = "Mainnet";
  pub testnet = "Testnet";
  pub xtz_amount = a => p("%s %s", a, this#xtz);
  pub xtz_op_amount = op => p("%s %a", op, () => this#xtz_amount);
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
  pub operation_summary_fee = p("+ Fee %a", () => this#xtz_amount);
  pub navbar_accounts = "ACCOUNTS";
  pub navbar_operations = "OPERATIONS";
  pub navbar_addressbook = {j|ADDRESS BOOK|j};
  pub navbar_delegations = "DELEGATIONS";
  pub navbar_tokens = "TOKENS";
  pub navbar_settings = "SETTINGS";
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
  pub upgrade_notice = "We recommend you upgrade your version of Umami."
};
