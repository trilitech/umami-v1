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
  pub _this = this;
  pub collected = "COLLECTED";
  pub gallery = "GALLERY";
  pub view_specs = "VIEW SPECS";
  pub goto_doc = "See help";
  pub goto_settings = "Go to settings";
  pub retry_network = "Retry";
  pub retry = "RETRY";
  pub view_nft = "View NFT";
  pub create_or_import_secret = "CREATE OR IMPORT SECRET";
  pub verify_accounts = "VERIFY ACCOUNTS";
  pub customize_derivation_path = "Customize Derivation Path";
  pub ardware_wallet_export = "Export Public Key";
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
  pub import_secret = "IMPORT EXISTING SECRET";
  pub create_secret_new = "CREATE NEW SECRET";
  pub connect_hardware_wallet = "CONNECT LEDGER";
  pub create_account_record_ok = {js|OK, I’VE RECORDED IT|js};
  pub add_account = "ADD ACCOUNT";
  pub add_contact = "ADD CONTACT";
  pub add_token = "REGISTER TOKEN";
  pub advanced_options = "ADVANCED OPTIONS";
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
  pub export_public_key = "EXPORT PUBLIC KEY";
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
  pub register_nft_contract = "Register NFT contract";
  pub see_list = "SEE LIST"
};

let tooltip = {
  pub close = "Close";
  pub _this = this;
  pub show_qr = "Show QR Code";
  pub copy_clipboard = "Copy to Clipboard";
  pub addressbook_edit = "Edit Contact";
  pub addressbook_delete = "Delete Contact";
  pub add_contact = "Add to Contacts";
  pub add_token = "Register token";
  pub unregistered_token_transaction = "This transaction was made using an unregistered token.";
  pub no_tez_no_delegation = "Delegation requires tez";
  pub refresh = "Refresh";
  pub open_in_explorer = "Open in explorer";
  pub update_delegation = "update delegation";
  pub chain_not_connected = "Not connected to a chain";
  pub custom_network_edit = "Edit network";
  pub custom_network_delete = "Delete network";
  pub reject_on_ledger = "Reject on ledger to cancel operation";
  pub see_network_info = "See network info";
  pub tokenid = {j|A non-negative number that identifies the asset inside a FA2 contract|j}
};

let log = {
  pub _this = this;
  pub copied_to_clipboard = p("%s copied to clipboard");
  pub log_content = "Log content";
  pub address = "Address";
  pub beacon_sign_payload = "Sign Payload"
};

let label = {
  pub _this = this;
  pub derivation_scheme = "Derivation Scheme";
  pub derivation_path = "Derivation Path";
  pub accounts = "Accounts";
  pub contract_address = "Contract address";
  pub advanced_options = "Advanced Options";
  pub recovery_phrase_format = "Recovery Phrase Format";
  pub account_create_name = "Name";
  pub add_contact_name = "Name";
  pub add_contact_address = "Address";
  pub send_amount = "Amount";
  pub send_sender = "Sender Account";
  pub send_recipient = "Recipient Account";
  pub send_nft = "NFT";
  pub password = "Password";
  pub fee = "Fee";
  pub implicit_reveal_fee = "Implicit Reveal Fee";
  pub gas_limit = "Gas Limit";
  pub storage_limit = "Storage Limit";
  pub counter = "Counter";
  pub force_low_fee = "Force low fee";
  pub confirm_password = "Confirm Password";
  pub account_delegate = "Account to delegate";
  pub baker = "Baker";
  pub add_token_address = "Address";
  pub add_token_name = "Name";
  pub add_token_symbol = "Symbol";
  pub add_token_decimals = "Decimals";
  pub add_token_id = "Token ID";
  pub summary_subtotal = "Subtotal";
  pub summary_total = "Total";
  pub summary_total_tez = "Total tez";
  pub transactions = "Transactions";
  pub account_secret = "Root";
  pub account_umami = "Umami";
  pub token = "Token";
  pub token_id = id => "Token ID: " ++ id;
  pub search_nft_by_name = "What are your wildest fears?";
  pub account_cli = "Cli";
  pub account_default_path = "Default Path - m/44'/1729'/?'/0'";
  pub account_custom_path = "Custom Path";
  pub beacon_client_name = "Umami";
  pub beacon_account = "Account to connect to dApp";
  pub beacon_sign_payload = "Payload to sign";
  pub beacon_dapp_pairing = "DApp pairing request";
  pub parameters = "Parameters";
  pub custom_network_name = "Name";
  pub custom_network_mezos_url = "Mezos URL";
  pub custom_network_node_url = plural => p("Node URL%s", plural ? "s" : "")
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
  pub add_token_decimals = "e.g. 0";
  pub add_token_id = "e.g. 0";
  pub enter_new_password = "Enter new password, at least 8 characters";
  pub confirm_password = "Confirm your new password";
  pub enter_password = "Enter your password";
  pub add_token_symbol = "e.g. tez, KLD, ...";
  pub enter_derivation_path = "Enter your derivation path";
  pub custom_network_name = "e.g. Test Network";
  pub search_for_nft = "Search for NFT by name";
  pub custom_network_node_url = "e.g. https://testnet-tezos.giganode.io/";
  pub custom_network_mezos_url = "e.g. https://api.umamiwallet.com/granadanet"
};

let form_input_error = {
  pub _this = this;
  pub dp_not_a_dp = "Not a derivation path";
  pub dp_more_than_1_wildcard = "Cannot have more than one '?'";
  pub dp_missing_wildcard = "Missing '?' or 0";
  pub dp_not_tezos = "Not a BIP44 Tezos Path";
  pub not_an_int = "This needs to be a number";
  pub negative_int = "This needs to be a natural number";
  pub nat = "must be a natural number";
  pub expected_decimals = n => p("allows at most %d decimals", n);
  pub hardware_wallet_not_ready = "Connection to Tezos ledger application failed. Please make sure the Tezos app is opened and the ledger unlocked.";
  pub hardware_wallet_plug = "The connection took too long.\nPlease make sure the Ledger is properly plugged in and unlocked";
  pub hardware_wallet_check_app = "Public key export failed. Please open the Tezos Wallet app and retry.";
  pub hardware_wallet_timeout = "Timeout while connecting to Ledger.\nPlug and unlock your Ledger then retry";
  pub hardware_wallet_signer_inconsistent = "Inconsistency between signing intent and key";
  pub empty_transaction = "Transaction is empty.";
  pub branch_refused_error = "Please retry. An error came up while communicating with the node";
  pub invalid_key_hash = "Invalid contract address";
  pub key_already_registered = a =>
    p("Address already registered under: %s", a);
  pub name_already_registered = "Name already registered";
  pub mandatory = "This input is mandatory";
  pub int = "must be an integer";
  pub float = "must be a float";
  pub change_baker = "must be a different baker than the current one";
  pub wrong_password = "Wrong password";
  pub password_length = "Password length must be at least 8 characters";
  pub unregistered_delegate = "This key is not registered as a baker";
  pub bad_pkh = "Not a valid key";
  pub invalid_contract = "The recipient is not a key or an alias";
  pub balance_too_low = "Balance is too low";
  pub script_rejected = "Script rejected";
  pub account_balance_empty = "Account is empty";
  pub confirm_password = "It must be the same password";
  pub derivation_path_error = "Invalid derivation path";
  pub name_already_taken = a => p("%s is already taken", a);
  pub api_not_available = "API not available";
  pub node_not_available = "Node not available";
  pub api_and_node_not_available = "API and node both not available";
  pub different_chains = "API and Node are not running on the same network";
  pub not_a_token_contract = "Not a standard token contract";
  pub no_metadata = pkh =>
    p(
      "No metadata was found for the contract%s.",
      pkh->Option.mapWithDefault("", p(": %s")),
    );
  pub no_token_metadata = pkh =>
    p(
      "No token metadata was found for the contract%s.",
      pkh->Option.mapWithDefault("", p(": %s")),
    );
  pub token_id_not_found =
    fun
    | None => "Requested token id was not found"
    | Some((pkh, tokenId)) =>
      p("Token id %d not found for contract %s", tokenId, pkh);
  pub illformed_token_metadata = (pkh, tokenId, field) =>
    p(
      "Field %s for token id %d illformed for contract %s",
      field,
      tokenId,
      pkh,
    );
  pub token_id_expected = "FA2 contracts expects a token id"
};

let title = {
  pub _this = this;
  pub error_logs = "Logs";
  pub secret_create = "Create New Secret";
  pub derive_account = "Add Account";
  pub account_update = "Edit Account";
  pub import_account = "Import Account";
  pub hardware_wallet_connect = "Connect Ledger";
  pub hardware_wallet_confirm = "Confirm on Ledger";
  pub hardware_wallet_op_confirm = "Ledger found, please confirm the operation on the device";
  pub hardware_wallet_confirmed = "Confirmed on Ledger";
  pub hardware_wallet_op_confirmed = "Transaction confirmed on Ledger";
  pub hardware_wallet_search = "Searching for Ledger";
  pub hardware_wallet_error_unknown = "Ledger unknown error";
  pub hardware_wallet_not_found = "Ledger not found";
  pub hardware_wallet_not_ready = "Ledger not ready";
  pub hardware_wallet_denied = "Operation rejected on Ledger";
  pub hardware_wallet_error_app = "Tezos application not started or export rejected";
  pub hardware_wallet_check_follow = "Follow the checklist";
  pub hardware_confirm_pkh = "Please confirm public key export on your Ledger.";
  pub secret_update = "Edit Secret";
  pub add_contact = "Add Contact";
  pub update_contact = "Edit Contact";
  pub add_token = "Register Token";
  pub export = "Export";
  pub account_create_password = "Set a password to secure your wallet";
  pub account_enter_password = "Please enter password to confirm";
  pub account_create_verify_phrase = "Verify your recovery phrase";
  pub account_derivation_path = "Derivation Path";
  pub import_account_enter_phrase = "Enter your recovery phrase";
  pub operation_submitted = "Operation Submitted";
  pub sender_account = "Sender Account";
  pub recipient_account = "Recipient Account";
  pub delegated_account = "Account to delegate";
  pub baker_account = "Baker";
  pub withdraw_baker = "Withdraw Baker";
  pub confirm_delegate = "Confirm Delegate";
  pub delegate = "Delegate";
  pub delegate_update = "Change Baker";
  pub delegate_delete = "End Delegation";
  pub delegation_sent = "Delegation Sent";
  pub baker_updated = "Baker Updated";
  pub delegation_deleted = "Delegation Deleted";
  pub delete_contact = "Delete Contact?";
  pub deleted_contact = "Contact Deleted";
  pub delete_token = "Delete Token";
  pub deleted_token = "Token Deleted";
  pub delete_beacon_peer = "Delete Peer";
  pub delete_beacon_permission = "Delete Permission";
  pub confirm_cancel = "Are you sure you want to cancel?";
  pub send = "Send";
  pub confirmation = "Confirmation";
  pub simulation = "Simulation";
  pub submitting = "Submitting Operation";
  pub send_many_transactions = "Send one or many transactions";
  pub delete_account = "Delete Account?";
  pub delete_secret = "Delete Secret?";
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
  pub beacon_error = "Beacon Error";
  pub accounts = "Accounts";
  pub operations = "Operations";
  pub addressbook = "Address Book";
  pub delegations = "Delegations";
  pub tokens = "Tokens";
  pub settings = "Settings";
  pub collected = "Collected";
  pub gallery = "Gallery";
  pub logs = "Logs"
};

let expl = {
  pub _this = this;
  pub nft_empty_state = "Umami should automatically discover any NFT you possess.";
  pub network_disconnect = "The Tezos network is currently unreachable. Your internet connection might be unstable. If it is not the case, you should check your configuration and update it by following the documentation";
  pub hardware_wallet_confirm_operation = "Please validate the details of the transaction and press Confirm to sign it on your Ledger.";
  pub hardware_wallet_advopt = "Umami wallet supports three derivation schemes as well as custom derivation path to select new addresses. You may also use the default derivation scheme & path by leaving the preselected values.";
  pub hardware_wallet_check_complete_steps = "Complete the steps in order to connect.";
  pub hardware_wallet_check_firmware = "Make sure your Ledger has the latest firmware version.";
  pub hardware_wallet_check_app = "Install and open the Tezos Wallet app on your Ledger.";
  pub hardware_wallet_check_plug = "Plug your Ledger into your computer using a USB cable.";
  pub hardware_wallet_check_unlock = "Unlock your Ledger.";
  pub hardware_wallet_check_click = "Click the EXPORT PUBLIC KEY button below and confirm the action on your Ledger.";
  pub hardware_wallet_confirmed = "Public key export confirmed";
  pub hardware_wallet_op_confirmed = "Operation signed and sent to the network.";
  pub hardware_wallet_confirm = "Ledger found. Please confirm public key export.";
  pub hardware_wallet_op_confirm = "Verify the operation details displayed on your Ledger before you sign.";
  pub hardware_wallet_denied = "The operation has been rejected from the Ledger device";
  pub hardware_wallet_search = "Please make sure to unlock your Ledger and open the Tezos Wallet app.";
  pub send_many_transactions = "You have the ability to submit a batch of transactions, but please note that the batch will be confirmed as a whole: should one transaction in the batch fail then the whole batch will not be completed.";
  pub scan = "A scan will check the Tezos chain for other accounts from your secrets that may have been revealed outside of this wallet.";
  pub secret_create_record_recovery = {j|Please record the following 24 words in sequence in order to restore it in the future. Ensure to back it up, keeping it securely offline.|j};
  pub secret_create_record_verify = {j|We will now verify that you’ve properly recorded your recovery phrase. To demonstrate this, please type in the word that corresponds to each sequence number.|j};
  pub secret_create_password_not_recorded = {j|Please note that this password is not recorded anywhere and only applies to this machine.|j};
  pub secret_select_derivation_path = {j|Umami wallet supports custom derivation path to select new addresses. You may also select the default derivation path and use the default key.|j};
  pub import_secret_enter_phrase = {j|Please fill in the recovery phrase in sequence.|j};
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
  pub operation_cancel_delegation = "Cancel Delegation";
  pub delegate_edit = "Change Baker";
  pub delegate_delete = "End Delegation";
  pub batch_edit = "Edit Transfer";
  pub batch_delete = "Delete Transfer";
  pub delete_account = "Delete";
  pub delete_secret = "Delete";
  pub edit = "Edit";
  pub scan = "Scan"
};

let settings = {
  pub _this = this;
  pub theme_title = "THEME";
  pub theme_system = "Default to system";
  pub theme_dark = "Dark";
  pub theme_light = "Light";
  pub confirmations_title = "VERIFICATION";
  pub confirmations_label = "Number of confirmations (blocks)";
  pub confirmations_saved = "Number of confirmations saved";
  pub chain_title = {j|CHAIN/NETWORK|j};
  pub chain_node_label = "Node URL";
  pub chain_mezos_label = "Mezos URL";
  pub chain_saved = "Chain URLs Saved";
  pub token_title = "TOKENS";
  pub token_label = "Token Balance Viewer Contract";
  pub token_saved = "Token Balance Viewer Contract Saved";
  pub danger_title = "DANGER ZONE";
  pub danger_reset_section = "Reset Settings";
  pub danger_reset_text = {js|This will remove or restore custom settings to default values.|js};
  pub danger_reset_button = "RESET";
  pub danger_reset_confirm_title = "Reset Settings";
  pub danger_reset_confirm_text = {js|This will remove or reset all customized settings to their defaults. Personnal data -including saved contacts, password and accounts- won't be affected.|js};
  pub danger_reset_confirm_button = "RESET SETTINGS";
  pub danger_offboard_section = "Offboard Wallet";
  pub danger_offboard_text = {js|Offboarding will permanently delete any data from this computer. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the backup phrase.|js};
  pub danger_offboard_button = "OFFBOARD";
  pub danger_offboard_form_title = "Offboard Wallet";
  pub danger_offboard_form_text = {js|Offboarding will permanently delete any data from this computer. Please acknowledge that you have read and understood the disclaimer, then enter « wasabi » to confirm. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the recovery phrase.|js};
  pub danger_offboard_form_checkbox_label = {js|I have read the warning and I am certain I want to delete my private keys locally. I also made sure to keep my recovery phrase.|js};
  pub danger_offboard_form_input_placeholder = "Enter code word to confirm";
  pub danger_offboard_form_input_error = "Not the correct confirm code word";
  pub beacon_title = "DAPPS";
  pub beacon_peers_section = "Peers";
  pub beacon_peers_name = "NAME";
  pub beacon_peers_relay = "RELAY SERVER";
  pub beacon_empty_peers = "No Peer";
  pub beacon_permissions_section = "Permissions";
  pub beacon_permissions_dapp = "DAPP";
  pub beacon_permissions_account = "CONNECTED ACCOUNT";
  pub beacon_permissions_scopes = "SCOPES";
  pub beacon_permissions_network = "NETWORK";
  pub beacon_empty_permissions = "No Permission"
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
  pub api_error = n => p("API error: received %d", n);
  pub invalid_checksum = "Invalid checksum";
  pub invalid_length = "Invalid length";
  pub valid = "Valid";
  pub unknown_error_code = n => p("Unknown error code %d", n)
};

let wallet = {
  pub _this = this;
  pub key_not_found = "Key not found";
  pub key_bad_format = s => p("Can't readkey, bad format: %s", s);
  pub invalid_path_size =
    p("Path %s is not valid: it must be of at least of two indexes");
  pub invalid_tezos_prefix_path = (prefix, index) =>
    p(
      "Prefix %s at index %d is not valid: a Tezos path must start with 44'/1729'",
      prefix,
      index,
    );
  pub invalid_index = (index, value) =>
    p("Value %s at index %d is invalid for a derivation path", value, index);
  pub invalid_prefix = prefix =>
    p("%s is not a valid prefix for an encoded Ledger key", prefix);
  pub invalid_scheme = scheme =>
    p("%s is not a valid scheme for an encoded Ledger key", scheme);
  pub invalid_encoding = enc =>
    p("%s is not a valid an encoded Ledger key", enc);
  pub invalid_ledger =
    p("The Ledger connected has base key %s, which is not the one expected.")
};

let errors = {
  pub _this = this;
  pub download_error_status = p("Request failed with status code %d");
  pub download_error = "Error during file download";
  pub invalid_estimation_results = "Invalid estimation results";
  pub request_to_node_failed = "Request to node failed";
  pub every_balances_fail = "Every balances fail to load";
  pub network_unreachable = "Tezos network unreachable.";
  pub no_valid_endpoint = "No valid endpoint found among public nodes";
  pub not_an_fa12_contract = "Not an FA1.2 standard contract";
  pub beacon_cant_handle = "Cannot handle this operation";
  pub cannot_retrieve_account = "Cannot retrieve account informations";
  pub deeplinking_not_connected = "Deep linking required but not connected";
  pub stream = "Acquiering media failed";
  pub decryption = "Storage decryption failed";
  pub encryption = "Storage encryption failed";
  pub key_derivation = "Key derivation failed";
  pub illformed_token_contract = "Illformed Token Contract";
  pub cannot_read_token = s => p("Cannot read token amount: %s", s);
  pub unknown_kind = k =>
    p("Internal error: unknown kind `%s` for token contract", k);
  pub invalid_operation_type = "Invalid operation type!";
  pub unhandled_error = e => p("Unhandled error %s", e);
  pub no_secret_found = "No secrets found";
  pub secret_not_found = i => p("Secret at index %d not found!", i);
  pub cannot_update_secret = i => p("Can't update secret at index %d!", i);
  pub recovery_phrase_not_found = i =>
    p("Recovery phrase at index %d not found!", i);
  pub beacon_transaction_not_supported = "Beacon transaction not supported";
  pub beacon_request_network_missmatch = "Beacon request network not supported";
  pub beacon_client_not_created = "Beacon client not created";
  pub video_stream_access_denied = "Unable to access video stream\n(please make sure you have a webcam enabled)";
  pub incorrect_number_of_words = "Mnemonic must have 12, 15 or 24 words.";
  pub pairing_request_parsing = "Illformed pairing request";
  pub unknown_bip39_word = (w, i) =>
    p(
      "Word %d ('%s') is not a valid BIP39 word, please refer to the standard.",
      i,
      w,
    );
  pub secret_already_imported = "Secret already imported";
  pub json_parsing_error = _ => "JSON parsing error";
  pub local_storage_key_not_found = k =>
    p("Internal error: key `%s` not found", k);
  pub version_format = v =>
    p("Internal error: invalid version format `%s`", v);
  pub storage_migration_failed = v =>
    p("Internal error: storage migration failed at version %s", v);
  pub unknown_network = c => p("No public network exists for chain %s", c)
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
  pub no_balance_amount = "---- ";
  pub logs_no_recent = "No Recent Message";
  pub logs_clearall = "CLEAR ALL";
  pub amount = (a, b) => p("%s %s", a, b);
  pub tezos = "Tez";
  pub tez = "tez";
  pub you_dont_have_nft = "You don't have any NFT yet";
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
  pub contact_added = "Contact Added";
  pub contact_updated = "Contact Updated";
  pub contact_deleted = "Contact Deleted";
  pub account_created = "Account Created";
  pub account_updated = "Account Updated";
  pub secret_updated = "Secret Updated";
  pub secret_deleted = "Secret Deleted";
  pub token_contract = p("%s Token Contract");
  pub token_created = "Token Created";
  pub token_deleted = "Token Deleted";
  pub operation_hash = "Operation Hash";
  pub operation_summary_fee = p("+ Fee %a", () => this#tez_amount);
  pub navbar_accounts = "ACCOUNTS";
  pub navbar_nft = "NFT";
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
  pub empty_delegations = "No Delegation";
  pub empty_operations = "No Operation";
  pub empty_address_book = "No Contact";
  pub add_token_format_contract_sentence = {j|Please enter the address of a deployed token contract for which you would like to view balances as well as to perform operations.|j};
  pub add_token_contract_metadata_fa1_2 = {j|Please specify the name, symbol, and decimals of a token contract for which you would like to view balances as well as to perform operations. Umami will prefill the fields if any metadata is available.|j};
  pub add_token_contract_tokenid_fa2 = {j|Please specify the token ID of the token you would like to perform operations on. Umami will prefill the fields if any metadata is available.|j};
  pub add_token_contract_metadata_fa2 = {j|Please specify the token ID of the token you would like to perform operations on. Umami will prefill the fields if any metadata is available|j};
  pub delegation_removal = "Delegation Removal";
  pub error_check_contract = "Address is not a valid token contract";
  pub words = p("%d words");
  pub upgrade_notice = "We recommend you upgrade your version of Umami.";
  pub custom_network_created = "Network created";
  pub custom_network_updated = "Network updated";
  pub custom_network_deleted = "Network deleted";
  pub unregistered_token = "unregistered token";
  pub hw = "H/W"
};
