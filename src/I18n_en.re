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

module Btn = {
  let json = "JSON";
  let metadata = "METADATA";
  let collected = "COLLECTED";
  let gallery = "GALLERY";
  let view_specs = "VIEW SPECS";
  let goto_doc = "See help";
  let goto_settings = "Go to settings";
  let retry_network = "Retry";
  let retry = "RETRY";
  let view_nft = "View NFT";
  let create_or_import_secret = "CREATE OR IMPORT SECRET";
  let buy_tez = "BUY TEZ";
  let verify_accounts = "VERIFY ACCOUNTS";
  let customize_derivation_path = "Customize Derivation Path";
  let ok = "OK";
  let cancel = "CANCEL";
  let continue = "CONTINUE";
  let delete = "DELETE";
  let finish = "FINISH";
  let add = "ADD";
  let send = "SEND";
  let save = "SAVE";
  let done_ = "SIMPLE VIEW";
  let edit = "MANAGEMENT VIEW";
  let go_back = "GO BACK";
  let go_operations = "GO TO OPERATIONS";
  let confirm = "CONFIRM";
  let update = "UPDATE";
  let save_and_register = "SAVE AND REGISTER";
  let import_secret = "IMPORT SECRET WITH RECOVERY PHRASE";
  let create_secret_new = "CREATE NEW SECRET";
  let connect_hardware_wallet = "CONNECT LEDGER";
  let create_account_record_ok = {js|OK, I’VE RECORDED IT|js};
  let add_account = "ADD ACCOUNT";
  let add_contact = "ADD CONTACT";
  let add_token = "REGISTER TOKEN";
  let advanced_options = "ADVANCED OPTIONS";
  let delegate = "DELEGATE";
  let add_transaction = "ADD TRANSACTION";
  let send_submit = "SUBMIT TRANSACTION";
  let batch_submit = "SUBMIT BATCH";
  let send_cancel = "CANCEL TRANSACTION";
  let delegation_submit = "CONFIRM DELEGATION";
  let delegated = "DELEGATED";
  let delete_token = "Remove from wallet";
  let import = "IMPORT";
  let export = "EXPORT";
  let export_public_key = "EXPORT PUBLIC KEY";
  let scan = "SCAN";
  let upgrade = "Download";
  let load_file = "LOAD FROM FILE";
  let csv_format_link = "SEE FILE SPECS";
  let disclaimer_agree = "CONTINUE";
  let deny = "DENY";
  let allow = "ALLOW";
  let reject = "REJECT";
  let close = "CLOSE";
  let add_custom_network = "ADD CUSTOM NETWORK";
  let beacon_connect_dapp = "CONNECT TO DAPP";
  let custom = "CUSTOM";
  let see_list = "SEE LIST";
  let upload_file = "UPLOAD FILE";
  let access_wallet = "ACCESS WALLET";
  let restore_secret = "RESTORE FROM BACKUP";
  let browse_for_folder = "BROWSE FOR FOLDER";
  let setup_later = "Set up later in settings";
};

module Tooltip = {
  let close = "Close";
  let show_qr = "Show QR Code";
  let copy_clipboard = "Copy to Clipboard";
  let addressbook_edit = "Edit Contact";
  let addressbook_delete = "Delete Contact";
  let add_contact = "Add to Contacts";
  let add_token = "Register token";
  let unregistered_token_transaction = "This transaction was made using an unregistered token.";
  let no_tez_no_delegation = "Delegation requires tez";
  let refresh = "Refresh";
  let sync = "Sync";
  let stop_sync = "Stop sync";
  let open_in_explorer = "Open in explorer";
  let update_delegation = "update delegation";
  let chain_not_connected = "Not connected to a chain";
  let custom_network_edit = "Edit network";
  let custom_network_delete = "Delete network";
  let reject_on_ledger = "Reject on ledger to cancel operation";
  let see_network_info = "See network info";
  let tokenid = {j|A non-negative number that identifies the asset inside a FA2 contract|j};
  let add_to_wallet = "Add to wallet";
  let reject_on_provider = p("Reject on %s popup to cancel operation");
};

module Log = {
  let copied_to_clipboard = p("%s copied to clipboard");
  let log_content = "Log content";
  let address = "Address";
  let content = "Content";
  let beacon_sign_payload = "Sign Payload";
};

module Label = {
  let provider_account = "account";
  let tezos_address = "Tezos address";
  let tz_address = "tz address";
  let nft_spec_name = "Name";
  let nft_spec_symbol = "Symbol";
  let nft_spec_contract_address = "Contract Address";
  let nft_spec_token_id = "Token ID";
  let nft_spec_decimals = "Decimals";
  let nft_spec_boolean_amount = "Boolean Amount";
  let nft_spec_creators = "Creators";
  let nft_spec_thumbnail_uri = "Thumbnail URI";
  let nft_spec_artifact_uri = "Artifact URI";
  let nft_spec_display_uri = "Display URI";
  let nft_spec_formats = "Formats";
  let nft_spec_description = "Description";
  let derivation_scheme = "Derivation Scheme";
  let derivation_path = "Derivation Path";
  let accounts = "Accounts";
  let contract_address = "Contract address";
  let advanced_options = "Advanced Options";
  let recovery_phrase_format = "Recovery Phrase Format";
  let account_create_name = "Name";
  let add_contact_name = "Name";
  let add_contact_address = "Address";
  let send_amount = "Amount";
  let send_sender = "Sender Account";
  let send_recipient = "Recipient Account";
  let send_nft = "NFT";
  let password = "Password";
  let fee = "Fee";
  let implicit_reveal_fee = "Implicit Reveal Fee";
  let gas_limit = "Gas Limit";
  let storage_limit = "Storage Limit";
  let confirm_password = "Confirm Password";
  let account_delegate = "Account to delegate";
  let baker = "Baker";
  let add_token_address = "Address";
  let add_token_name = "Name";
  let add_token_symbol = "Symbol";
  let add_token_decimals = "Decimals";
  let add_token_id = "Token ID";
  let summary_subtotal = "Subtotal";
  let summary_total = "Total";
  let summary_total_tez = "Total tez";
  let account_secret = "Root";
  let account_umami = "Umami";
  let token = "Token";
  let token_id = id => "Token ID: " ++ id;
  let editions = number => "Editions: " ++ number;
  let account_default_path = "Default Path - m/44'/1729'/?'/0'";
  let account_custom_path = "Custom Path";
  let beacon_client_name = "Umami";
  let beacon_account = "Account to connect to dApp";
  let beacon_sign_payload = "Payload to sign";
  let beacon_dapp_pairing = "DApp pairing request";
  let parameters = "Parameters";
  let storage = "Storage";
  let code = "Code";
  let custom_network_name = "Name";
  let custom_network_mezos_url = "Mezos URL";
  let custom_network_node_url = plural => p("Node URL%s", plural ? "s" : "");
  let file = "File";
  let storage_location = "Storage location";
  let entrypoint = "Entrypoint";
  let parameter = "Parameter";
};

module Input_placeholder = {
  let provider_handler = (provider, handlerKind) =>
    p("Type in a %s %s", provider, handlerKind);
  let tez_amount = "0.000000";
  let token_amount = "0";
  let add_accounts_name = "Enter account's name";
  let add_contacts_name = "Enter contact's name";
  let add_contacts_tz = "Enter contact's tz address";
  let add_contact_or_tz = "Paste a tz address or type in a contact's name";
  let add_token_address = "Enter KT1 address of a contract";
  let add_token_name = "e.g. Tezos";
  let add_token_decimals = "e.g. 0";
  let add_token_id = "e.g. 0";
  let enter_new_password = "Enter new password, at least 8 characters";
  let confirm_password = "Confirm your new password";
  let add_token_symbol = "e.g. tez, KLD, ...";
  let enter_derivation_path = "Enter your derivation path";
  let custom_network_name = "e.g. Test Network";
  let search_for_nft = "Search for NFT by name";
  let search_for_token = "Search for Token by name, symbol or KT address";
  let custom_network_node_url = "e.g. https://rpc.tzbeta.net";
  let custom_network_mezos_url = "e.g. https://api.umamiwallet.com/mainnet";
  let select_backup_path = "Browse to the required folder or enter a path";
};

module Form_input_error = {
  let invalid_handler = (provider, handlerKind) =>
    p("Invalid %s %s", provider, handlerKind);
  let dp_not_a_dp = "Not a derivation path";
  let dp_more_than_1_wildcard = "Cannot have more than one '?'";
  let dp_missing_wildcard = "Missing '?' or 0";
  let dp_not_tezos = "Not a BIP44 Tezos Path";
  let not_an_int = "This needs to be a number";
  let negative_int = "This needs to be a natural number";
  let expected_decimals = n => p("allows at most %d decimals", n);
  let hardware_wallet_not_ready = "Connection to Tezos ledger application failed. Please make sure the Tezos app is opened and the ledger unlocked.";
  let hardware_wallet_plug = "The connection took too long.\nPlease make sure the Ledger is properly plugged in and unlocked";
  let hardware_wallet_check_app = "Public key export failed. Please open the Tezos Wallet app and retry.";
  let hardware_wallet_timeout = "Timeout while connecting to Ledger.\nPlug and unlock your Ledger then retry";
  let hardware_wallet_signer_inconsistent = "Inconsistency between signing intent and key";
  let empty_transaction = "Transaction is empty.";
  let branch_refused_error = "Please retry. An error came up while communicating with the node";
  let invalid_key_hash = "Invalid contract address";
  let key_already_registered = a =>
    p("Address already registered under: %s", a);
  let name_already_registered = "Name already registered";
  let mandatory = "This input is mandatory";
  let int = "must be an integer";
  let float = "must be a float";
  let change_baker = "must be a different baker than the current one";
  let wrong_password = "Wrong password";
  let password_length = "Password length must be at least 8 characters";
  let unregistered_delegate = "This key is not registered as a baker";
  let bad_pkh = "Not a valid key";
  let invalid_contract = "The recipient is not a key or an alias";
  let balance_too_low = "Balance is too low";
  let account_balance_empty = "Account is empty";
  let confirm_password = "It must be the same password";
  let derivation_path_error = "Invalid derivation path";
  let name_already_taken = a => p("%s is already taken", a);
  let api_not_available = "API not available";
  let node_not_available = "Node not available";
  let different_chains = "API and Node are not running on the same network";
  let not_a_token_contract = "Not a standard token contract";
  let no_metadata = pkh =>
    p(
      "No metadata was found for the contract%s.",
      pkh->Option.mapWithDefault("", p(": %s")),
    );
  let no_token_metadata = pkh =>
    p(
      "No token metadata was found for the contract%s.",
      pkh->Option.mapWithDefault("", p(": %s")),
    );
  let token_id_not_found =
    fun
    | None => "Requested token id was not found"
    | Some((pkh, tokenId)) =>
      p("Token id %d not found for contract %s", tokenId, pkh);
  let illformed_token_metadata = (pkh, tokenId, field) =>
    p(
      "Field %s for token id %d illformed for contract %s",
      field,
      tokenId,
      pkh,
    );
};

module Title = {
  let custom_auth_sign_in_up = "Waiting for authentification";
  let custom_auth_waiting_auth = "Waiting for authentification";
  let custom_auth_failed = "Authentification failed";
  let custom_auth_success = "Successful authentification";
  let provider_authentification = pr => p("%s authentification", pr);
  let nft_specs = "View Specs";
  let secret_create = "Create New Secret";
  let derive_account = "Add Account";
  let account_update = "Edit Account";
  let import_account = "Import Account";
  let hardware_wallet_connect = "Connect Ledger";
  let hardware_wallet_confirm = "Confirm on Ledger";
  let hardware_wallet_op_confirm = "Ledger found, please confirm the operation on the device";
  let hardware_wallet_confirmed = "Confirmed on Ledger";
  let hardware_wallet_op_confirmed = "Transaction confirmed on Ledger";
  let hardware_wallet_search = "Searching for Ledger";
  let hardware_wallet_error_unknown = "Ledger unknown error";
  let hardware_wallet_not_found = "Ledger not found";
  let hardware_wallet_not_ready = "Ledger not ready";
  let hardware_wallet_denied = "Operation rejected on Ledger";
  let hardware_wallet_error_app = "Tezos application not started or export rejected";
  let hardware_wallet_check_follow = "Follow the checklist";
  let hardware_confirm_pkh = "Please confirm public key export on your Ledger.";
  let secret_update = "Edit Secret";
  let add_contact = "Add Contact";
  let update_contact = "Edit Contact";
  let add_token = "Register Token";
  let edit_metadata = "Token Metadata";
  let export = "Export";
  let account_create_password = "Set a password to secure your wallet";
  let account_enter_password = "Please enter password to confirm";
  let account_create_verify_phrase = "Verify your recovery phrase";
  let account_derivation_path = "Derivation Path";
  let import_account_enter_phrase = "Enter your recovery phrase";
  let operation_submitted = "Operation Submitted";
  let sender_account = "Sender Account";
  let baker_account = "Baker";
  let withdraw_baker = "Withdraw Baker";
  let confirm_delegate = "Confirm Delegate";
  let confirm_batch = "Confirm";
  let delegate = "Delegate";
  let delegate_update = "Change Baker";
  let delegate_delete = "End Delegation";
  let delete_contact = "Delete Contact?";
  let delete_beacon_peer = "Delete Peer";
  let delete_beacon_peers = "Delete Peers";
  let delete_beacon_permission = "Delete Permission";
  let delete_beacon_permissions = "Delete Permissions";
  let confirm_cancel = "Are you sure you want to cancel?";
  let send = "Send";
  let confirmation = "Confirmation";
  let send_many_transactions = "Send one or many transactions";
  let delete_account = "Delete Account?";
  let delete_batch = "Delete this batch?";
  let delete_secret = "Delete Secret?";
  let scan = "Scan";
  let disclaimer = "User Agreement";
  let beacon_connection_request = "Connection Request";
  let beacon_sign_request = "Sign";
  let beacon_pairing = "Connect to DApp with pairing request";
  let beacon_pairing_qr = "Connect to DApp with QR code";
  let beacon_scan_qr = "Scan QR code from dApp";
  let add_custom_network = "Add Custom Network";
  let update_custom_network = "Edit Custom Network";
  let delete_custom_network = "Delete Network?";
  let beacon_error = "Beacon Error";
  let accounts = "Accounts";
  let global_batch = "Batch";
  let global_batch_empty = "Your batch is currently empty";
  let operations = "Operations";
  let addressbook = "Address Book";
  let delegations = "Delegations";
  let tokens = "Tokens";
  let settings = "Settings";
  let collected = "Collected";
  let gallery = "Gallery";
  let notice = "Disclaimer Notice";
  let buy_tez = "Buy tez";
  let wert_recipient = "Please select the recipient account";
  let added_to_wallet = "Added to wallet";
  let held = "Held";
  let restore_account = "Restore from Backup";
  let select_backup_file = "Select a JSON backup file and enter the password you used to encrypt it.";
  let account_backup_path = "Select a storage location for your backup";
  let details = "Details";
};

module Expl = {
  let custom_auth_success = "Please click on confirm to submit operation";
  let custom_auth_sign = p("Please log into %s to sign the operation");
  let custom_auth_confirm_operation = "Please validate the details of the transaction and confirm it by signing up with your social account.";
  let provider_authentification = p("Please log into %s to import your key");
  let nft_empty_state = "Umami should automatically discover any NFT you possess.";
  let network_disconnect = "The Tezos network is currently unreachable. Your internet connection might be unstable. If it is not the case, you should check your configuration and update it by following the documentation";
  let hardware_wallet_confirm_operation = "Please validate the details of the transaction and press Confirm to sign it on your Ledger.";
  let hardware_wallet_advopt = "Umami wallet supports three derivation schemes as well as custom derivation path to select new addresses. You may also use the default derivation scheme & path by leaving the preselected values.";
  let hardware_wallet_check_complete_steps = "Complete the steps in order to connect.";
  let hardware_wallet_check_firmware = "Make sure your Ledger has the latest firmware version.";
  let hardware_wallet_check_app = "Install and open the Tezos Wallet app on your Ledger.";
  let hardware_wallet_check_plug = "Plug your Ledger into your computer using a USB cable.";
  let hardware_wallet_check_unlock = "Unlock your Ledger.";
  let hardware_wallet_check_click = "Click the EXPORT PUBLIC KEY button below and confirm the action on your Ledger.";
  let hardware_wallet_confirmed = "Public key export confirmed";
  let hardware_wallet_op_confirmed = "Operation signed and sent to the network.";
  let hardware_wallet_confirm = "Ledger found. Please confirm public key export.";
  let hardware_wallet_op_confirm = "Verify the operation details displayed on your Ledger before you sign.";
  let hardware_wallet_denied = "The operation has been rejected from the Ledger device";
  let hardware_wallet_search = "Please make sure to unlock your Ledger and open the Tezos Wallet app.";
  let scan = "A scan will check the Tezos chain for other accounts from your secrets that may have been revealed outside of this wallet.";
  let secret_create_record_recovery = {j|Please record the following 24 words in sequence in order to restore it in the future. Ensure to back it up, keeping it securely offline.|j};
  let secret_create_record_verify = {j|We will now verify that you’ve properly recorded your recovery phrase. To demonstrate this, please type in the word that corresponds to each sequence number.|j};
  let secret_create_password_not_recorded = {j|Please note that this password is not recorded anywhere and only applies to this machine.|j};
  let secret_select_derivation_path = {j|Umami wallet supports custom derivation path to select new addresses. You may also select the default derivation path and use the default key.|j};
  let import_secret_enter_phrase = {j|Please fill in the recovery phrase in sequence.|j};
  let confirm_operation = "Please validate the details of the transaction and enter password to confirm";
  let global_batch = "Please validate the details of the batch and sign the transaction.";
  let fill_batch = "Add elements via the send menu or load a CSV file";
  let delete_batch = "You are about to delete all the transactions included in the batch.";
  let operation = "The operation will be processed and confirmed, you can see its progress in the Operations section.";
  let beacon_dapp = "would like to connect to your wallet";
  let beacon_dapp_request = {js|This site is requesting access to view your account’s address.\nAlways make sure you trust the sites you interact with.|js};
  let beacon_operation = "Requests Operations";
  let beacon_dapp_sign = "requests your signature";
  let custom_network = "Please specify the parameters of the network you want to connect to.";
  let external_service = "Notice: you are using Wert, which is an external service to Umami.";
  let secret_select_backup_path = {j|If you ever need to restore your wallet, you can do so with this backup file and your password.|j};
};

module Menu = {
  let app_menu_support = "Support";
  let app_menu_help = "Help";
  let app_menu_new_version = "Check for new versions";
  let app_menu_website = "Website";
  let delegate_edit = "Change Baker";
  let delegate_delete = "End Delegation";
  let batch_edit = "Edit Transfer";
  let batch_delete = "Delete Transfer";
  let delete_account = "Delete";
  let delete_secret = "Delete";
  let edit = "Edit";
  let scan = "Scan";
  let view_in_explorer = "View in Explorer";
  let see_metadata = "See Metadata";
  let see_details = "See details";
};

module Settings = {
  let theme_title = "THEME";
  let theme_system = "Default to system";
  let theme_dark = "Dark";
  let theme_light = "Light";
  let confirmations_title = "VERIFICATION";
  let confirmations_label = "Number of confirmations (blocks)";
  let confirmations_saved = "Number of confirmations saved";
  let chain_title = {j|CHAIN/NETWORK|j};
  let danger_title = "DANGER ZONE";
  let danger_reset_section = "Reset Settings";
  let danger_reset_text = {js|This will remove or restore custom settings to default values.|js};
  let danger_reset_button = "RESET";
  let danger_reset_confirm_title = "Reset Settings";
  let danger_reset_confirm_text = {js|This will remove or reset all customized settings to their defaults. Personnal data -including saved contacts, password and accounts- won't be affected.|js};
  let danger_reset_confirm_button = "RESET SETTINGS";
  let danger_offboard_section = "Offboard Wallet";
  let danger_offboard_text = {js|Offboarding will permanently delete any data from this computer. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the backup phrase.|js};
  let danger_offboard_button = "OFFBOARD";
  let danger_offboard_form_title = "Offboard Wallet";
  let danger_offboard_form_text = {js|Offboarding will permanently delete any data from this computer. Please acknowledge that you have read and understood the disclaimer, then enter « wasabi » to confirm. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the recovery phrase.|js};
  let danger_offboard_form_checkbox_label = {js|I have read the warning and I am certain I want to delete my private keys locally. I also made sure to keep my recovery phrase.|js};
  let danger_offboard_form_input_placeholder = "Enter code word to confirm";
  let danger_offboard_form_input_error = "Not the correct confirm code word";
  let beacon_title = "DAPPS";
  let beacon_peers_section = "Peers";
  let beacon_peers_name = "NAME";
  let beacon_peers_relay = "RELAY SERVER";
  let beacon_peers_remove_all = "REMOVE ALL";
  let beacon_empty_peers = "No Peer";
  let beacon_permissions_section = "Permissions";
  let beacon_permissions_dapp = "DAPP";
  let beacon_permissions_account = "CONNECTED ACCOUNT";
  let beacon_permissions_scopes = "SCOPES";
  let beacon_permissions_network = "NETWORK";
  let beacon_permissions_remove_all = "REMOVE ALL";
  let beacon_empty_permissions = "No Permission";
  let backup_title = "WALLET BACKUP";
  let backup_text = "Create and save a backup file for the entire wallet";
  let backup_path_saved = "Backup file path saved";
  ();
};

module Network = {
  let api_not_available = "The API is not available";
  let api_version_rpc_error = a =>
    p("The API `/version` RPC parsing returned an error: %s", a);
  let api_version_format_error = a =>
    p(
      "The API `/version` RPC parsing returned an unknown version format: %s",
      a,
    );
  let api_monitor_rpc_error = a =>
    p("The API `/monitor/blocks` RPC parsing returned an error: %s", a);
  let node_not_available = "The node is not available";
  let node_version_rpc_error = a =>
    p("Node `/chains/main/chain_id` RPC parsing returned an error: %s", a);
  let chain_inconsistency = (a, b) =>
    p(
      "The API and Node are not running on the same network.\n\
     API runs on chain `%s` and the node on the chain `%s`.",
      a,
      b,
    );
  let unknown_chain_id = a =>
    p(
      "Network %s is not supported, your operation cannot be viewed in an explorer.",
      a,
    );
  let unknown_explorer = a => p("No explorer is known on network %s", a);
  let api_not_supported = a =>
    p("The API %s is not supported by this version of Umami.", a);
  let api_and_node_not_available = "API and Node are both not available";
  ();
};

module Taquito = {
  let not_an_account = "Not a tz address";
  let not_a_contract = "Not a contract address";
  let no_prefix_matched = "Unknown address prefix";
  let api_error = n => p("API error: received %d", n);
  let invalid_checksum = "Invalid checksum";
  let invalid_length = "Invalid length";
};

module Wallet = {
  let key_not_found = "Key not found";
  let key_bad_format = s => p("Can't readkey, bad format: %s", s);
  let invalid_path_size =
    p("Path %s is not valid: it must be of at least of two indexes");
  let invalid_index = (index, value) =>
    p("Value %s at index %d is invalid for a derivation path", value, index);
  let invalid_scheme = scheme =>
    p("%s is not a valid scheme for an encoded Public key", scheme);
  let invalid_encoding = enc =>
    p("%s is not a valid an encoded Public key", enc);
  let invalid_ledger =
    p("The Ledger connected has base key %s, which is not the one expected.");
  ();
};

module Errors = {
  let invalid_estimation_results = "Invalid estimation results";
  let request_to_node_failed = "Request to node failed";
  let every_balances_fail = "Every balances fail to load";
  let network_unreachable = "Tezos network unreachable.";
  let no_valid_endpoint = "No valid endpoint found among public nodes";
  let beacon_cant_handle = "Cannot handle this operation";
  let customauth_unable_to_retrieve_handle = "Unable to retrieve user handle";
  let customauth_popup_closed = "User closed popup during authentification";
  let customauth_handle_mismatch = p("Signing handle mismatch: %s & %s");
  let invalid_provider = s => p("%s is not a valid provider", s);
  let deeplinking_not_connected = "Deep linking required but not connected";
  let stream = "Acquiering media failed";
  let decryption = "Storage decryption failed";
  let encryption = "Storage encryption failed";
  let key_derivation = "Key derivation failed";
  let illformed_token_contract = "Illformed Token Contract";
  let cannot_read_token = s => p("Cannot read token amount: %s", s);
  let unknown_kind = k =>
    p("Internal error: unknown kind `%s` for token contract", k);
  let invalid_operation_type = "Invalid operation type!";
  let unhandled_error = e => p("Unhandled error %s", e);
  let no_secret_found = "No secrets found";
  let secret_not_found = i => p("Secret at index %d not found!", i);
  let cannot_update_secret = i => p("Can't update secret at index %d!", i);
  let recovery_phrase_not_found = i =>
    p("Recovery phrase at index %d not found!", i);
  let beacon_operation_not_supported = "Beacon operation not supported";
  let beacon_request_network_missmatch = "Beacon request network not supported";
  let beacon_client_not_created = "Beacon client not created";
  let video_stream_access_denied = "Unable to access video stream\n(please make sure you have a webcam enabled)";
  let incorrect_number_of_words = "Mnemonic must have 12, 15 or 24 words.";
  let unknown_bip39_word = (w, i) =>
    p(
      "Word %d ('%s') is not a valid BIP39 word, please refer to the standard.",
      i,
      w,
    );
  let secret_already_imported = "Secret already imported";
  let json_parsing_error = _ => "JSON parsing error";
  let local_storage_key_not_found = k =>
    p("Internal error: key `%s` not found", k);
  let version_format = v =>
    p("Internal error: invalid version format `%s`", v);
  let storage_migration_failed = v =>
    p("Internal error: storage migration failed at version %s", v);
  let unknown_network = c => p("No public network exists for chain %s", c);
  let script_parsing = e =>
    p(
      "Error when parsing script%s",
      e->Option.mapDefault("", m => "with message: " ++ m),
    );
  let micheline_parsing = e =>
    p(
      "Error when parsing Micheline%s",
      e->Option.mapDefault("", m => "with message: " ++ m),
    );
  let unknown_version = (current, expected) =>
    p("Unknown version %s, while %s expected", current, expected);
  let version_not_in_bound = (lowest, highest, version) =>
    p(
      "Version %s is not in the range of %s and %s",
      version,
      lowest,
      highest,
    );
  let unknown_backup_version = v =>
    p(
      "Version %s of backup file cannot be imported in the current version of Umami.",
      v,
    );
  let cannot_parse_version = err => p("Invalid backup file:\n%s", err);
  let gas_exhausted = "Gas exhausted for the operation, please put a higher limit";
  let storage_exhausted = "Storage exhausted for the operation, please put a higher limit";
  let gas_exhausted_above_limit = "Gas exhausted and above protocol limit per operation";
  let storage_exhausted_above_limit = "Storage exhausted and above protocol limit per operation";
  let not_a_number = v => p("%s is not a valid number", v);
  let negative_number = v => p("%s is negative", v);
  let unexpected_decimals = v => p("%s does not accept decimals", v);
  let expected_decimals = (v, d) =>
    p("%s does not accept more that %d decimals", v, d);
};

module Csv = {
  let cannot_parse_number = (row, col) =>
    p("Value at row %d column %d is not a number", row, col);
  let cannot_parse_boolean = (row, col) =>
    p("Value at row %d column %d is not a boolean", row, col);
  let cannot_parse_custom_value = (err, row, col) =>
    p("Value at row %d column %d is not valid:\n%s", row, col, err);
  let cannot_parse_row = row =>
    p("Row %d is not valid, some columns are probably missing", row);
  let cannot_parse_csv = p("CSV is not valid");
  let no_rows = p("CSV is empty");
  let cannot_parse_token_amount = (v, row, col) =>
    p(
      "Value %s at row %d column %d is not a valid token amount",
      ReBigNumber.toString(v),
      row,
      col,
    );
  let cannot_parse_tez_amount = (v, row, col) =>
    p(
      "Value %s at row %d column %d is not a valid tez amount",
      ReBigNumber.toString(v),
      row,
      col,
    );
  let unknown_token = (pkh, id) =>
    p(
      "Unknown token %s%s",
      pkh,
      id->Option.mapWithDefault("", p(" and tokenId %d")),
    );
  let cannot_parse_address = (a, reason) =>
    p("%s in not a valid address: %s.", a, reason);
  let fa1_2_invalid_token_id = pkh =>
    p("Contract %s is an FA1.2 token, it cannot have a token id", pkh);
  let fa2_invalid_token_id = pkh =>
    p("Contract %s is an FA2 token, it must have a token id", pkh);
};

module Disclaimer = {
  let last_updated = date => p("Last updated %s.", date);
  let please_sign = "In order to use Umami, \nyou must agree to the terms.";
  let agreement_checkbox = "Check here to indicate that you have read and \
     agree to the terms of the User Agreement";
};

let unknown_operation = "Unknown";
let error404 = "404 - Route Not Found :(";
let no_balance_amount = "---- ";
let logs_no_recent = "No Recent Message";
let logs_clearall = "CLEAR ALL";
let amount = (a, b) => p("%s %s", a, b);
let tezos = "Tez";
let tez = "tez";
let you_dont_have_nft = "You don't have any NFT yet";
let tez_amount = a => p("%s %s", a, tez);
let tez_op_amount = op => p("%s %a", op, () => tez_amount);
let account = "Account";
let operation_reveal = "Reveal";
let operation_transaction = "Transaction";
let operation_contract_call = "Call";
let operation_origination = "Origination";
let operation_delegation = "Delegation";
let operation_token_batch = "Token batch";
let operation_column_type = "TYPE";
let operation_column_amount = "AMOUNT";
let operation_column_recipient = "RECIPIENT";
let operation_column_timestamp = "TIMESTAMP";
let operation_column_fee = "FEE";
let operation_column_sender = "SENDER";
let operation_column_status = "STATUS";
let global_batch_column_type = "TYPE";
let global_batch_subject = "SUBJECT";
let global_batch_recipient = "RECIPIENT";
let global_batch_fee = "FEE";
let global_batch_delete_all = "DELETE ALL";
let global_batch_add = "INSERT TRANSACTION INTO BATCH";
let state_mempool = "Mempool";
let state_levels = p("%d/%d blocks");
let state_confirmed = "Confirmed";
let stepof = p("Step %d of %d");
let optional_stepof = p("Step %d of %d (optional)");
let account_create_record_recovery = "Record your recovery phrase";
let contact_added = "Contact Added";
let contact_updated = "Contact Updated";
let contact_deleted = "Contact Deleted";
let account_created = "Account Created";
let account_updated = "Account Updated";
let secret_updated = "Secret Updated";
let secret_deleted = "Secret Deleted";
let token_contract = p("%s Token Contract");
let token_created = "Token added to wallet";
let token_deleted = "Token removed from wallet";
let operation_hash = "Operation Hash";
let navbar_accounts = "ACCOUNTS";
let navbar_nft = "NFT";
let navbar_operations = "OPERATIONS";
let navbar_addressbook = {j|ADDRESS BOOK|j};
let navbar_delegations = "DELEGATIONS";
let navbar_tokens = "TOKENS";
let navbar_settings = "SETTINGS";
let navbar_logs = "LOGS";
let navbar_global_batch = "BATCH";
let delegate_column_account = "ACCOUNT";
let delegate_column_initial_balance = "INITIAL BALANCE";
let delegate_column_current_balance = "CURRENT BALANCE";
let delegate_column_duration = "DURATION";
let delegate_column_last_reward = "LAST REWARD";
let delegate_column_baker = "BAKER";
let token_column_standard = "STANDARD";
let token_column_name = "NAME";
let token_column_symbol = "SYMBOL";
let token_column_address = "ADDRESS";
let token_column_tokenid = "TOKEN ID";
let na = "N/A";
let empty_held_token = "No token held and not registered on the current chain";
let empty_delegations = "No Delegation";
let empty_operations = "No Operation";
let empty_address_book = "No Contact";
let add_token_format_contract_sentence = {j|Please enter the address of a deployed token contract for which you would like to view balances as well as to perform operations.|j};
let add_token_contract_metadata_fa1_2 = {j|Please specify the name, symbol, and decimals of a token contract for which you would like to view balances as well as to perform operations. Umami will prefill the fields if any metadata is available.|j};
let add_token_contract_tokenid_fa2 = {j|Please specify the token ID of the token you would like to perform operations on. Umami will prefill the fields if any metadata is available.|j};
let add_token_contract_metadata_fa2 = {j|Please specify the token ID of the token you would like to perform operations on. Umami will prefill the fields if any metadata is available|j};
let delegation_removal = "Delegation Removal";
let error_check_contract = "Address is not a valid token contract";
let error_register_not_fungible = "Cannot register an NFT as a fungible token";
let error_register_not_non_fungible = "Cannot register a fungible token as an NFT";
let words = p("%d words");
let upgrade_notice = "We recommend you upgrade your version of Umami.";
let custom_network_created = "Network created";
let custom_network_updated = "Network updated";
let custom_network_deleted = "Network deleted";
let hw = "H/W";
let email = "email";
let username = "username";
