let p = Format.sprintf;

let btn = {
  pub _this = this;
  pub ok = "OK";
  pub cancel = "CANCEL";
  pub delete = "DELETE";
  pub create = "CREATE";
  pub finish = "FINISH";
  pub add = "ADD";
  pub send = "SEND";
  pub confirm = "CONFIRM";
  pub update = "UPDATE";
  pub error_logs = "ERROR LOGS";
  pub import_account = "IMPORT EXISTING ACCOUNT";
  pub create_account_new = "CREATE NEW ACCOUNT";
  pub create_account_record_ok = {js|OK, I’VE RECORDED IT|js};
  pub add_contact = "ADD CONTACT";
  pub delete_contact = "Delete contact";
  pub advanced_options = "Advanced options";
  pub delegate = "DELEGATE";
  pub delegated = "DELEGATED"
};

let label = {
  pub _this = this;
  pub account_create_name = "Name";
  pub add_contact_name = "Name";
  pub add_contact_address = "Address";
  pub send_amount = "Amount";
  pub send_sender = "Sender account";
  pub send_recipient = "Recipient account";
  pub password = "Password";
  pub fee = "Fee";
  pub gas_limit = "Gas limit";
  pub storage_limit = "Storage limit";
  pub counter = "Counter";
  pub force_low_fee = "Force low fee";
  pub confirm_password = "Confirm password";
  pub account_delegate = "Account to delegate";
  pub baker = "Baker"
};

let title = {
  pub _this = this;
  pub error_logs = "Error logs";
  pub account_create = "Create new account";
  pub import_account = "Import account";
  pub add_contact = "Add contact";
  pub account_create_password = "Set a password to secure your wallet";
  pub account_create_verify_phrase = "Verify your recovery phrase";
  pub import_account_enter_phrase = "Enter your recovery phrase";
  pub operation_injected = "Operation injected in the node";
  pub sender_account = "Sender account";
  pub recipient_account = "Recipient account";
  pub delegated_account = "Account to delegate";
  pub baker_account = "Baker";
  pub delegate = "Delegate";
  pub delegate_update = "Change baker";
  pub delegate_delete = "End delegation";
  pub delegation_sent = "Delegation sent";
  pub baker_updated = "Baker updated";
  pub delegation_deleted = "Delegation deleted";
  pub delete_contact = "Delete contact?";
  pub deleted_contact = "Contact deleted";
  pub send = "Send"
};

let expl = {
  pub _this = this;
  pub account_create_record_recovery = {j|Please record the following 24 words in sequence in order to restore it in the future. Ensure to back it up, keeping it securely offline.|j};
  pub account_create_record_verify = {j|We will now verify that you’ve properly recorded your recovery phrase. To demonstrate this, please type in the word that corresponds to each sequence number.|j};
  pub account_create_password_not_recorded = {j|Please note that this password is not recorded anywhere and only applies to this machine.|j};
  pub import_account_enter_phrase = {j|Please fill in the recovery phrase in sequence.|j}
};

let t = {
  pub error404 = "404 - Route Not Found :(";
  pub navbar_add_account = "ADD ACCOUNT";
  pub error_logs_no_recent = "No recent errors";
  pub error_logs_clearall = "CLEAR ALL";
  pub xtz = "XTZ";
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
  pub state_in_mempool = "in mempool";
  pub state_in_chain = "in chain";
  pub stepof = p("Step %d of %d");
  pub account_create_record_recovery = "Record your recovery phrase";
  pub contact_added = "Contact added";
  pub account_created = "Account created";
  pub operation_hash = "Operation hash";
  pub account_imported = "Account imported";
  pub operation_menu_register_as_baker = "Register as new baker";
  pub operation_menu_delegate_to = p("Delegate_to %s");
  pub operation_menu_unknown = "Unknown";
  pub operation_menu_cancel_delegation = "Cancel delegation";
  pub addressbook_menu_edit = "Edit contact";
  pub operation_summary_fee = p("+ Fee %a", () => this#xtz_amount);
  pub navbar_accounts = "ACCOUNTS";
  pub navbar_operations = "OPERATIONS";
  pub navbar_addressbook = {j|ADDRESS\nBOOK|j};
  pub navbar_delegations = "DELEGATIONS";
  pub delegate_column_account = "ACCOUNT";
  pub delegate_column_initial_balance = "INITIAL BALANCE";
  pub delegate_column_current_balance = "CURRENT BALANCE";
  pub delegate_column_duration = "DURATION";
  pub delegate_column_last_reward = "LAST REWARD";
  pub delegate_column_baker = "BAKER";
  pub delegate_menu_edit = "Change baker";
  pub delegate_menu_delete = "End delegation"
};
