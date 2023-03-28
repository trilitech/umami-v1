/* *************************************************************************** */
/*  */
/* Open Source License */
/* Copyright (c) 2019-2022 Nomadic Labs, <contact@nomadic-labs.com> */
/*  */
/* Permission is hereby granted, free of charge, to any person obtaining a */
/* copy of this software and associated documentation files (the "Software"), */
/* to deal in the Software without restriction, including without limitation */
/* the rights to use, copy, modify, merge, publish, distribute, sublicense, */
/* and/or sell copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following conditions: */
/*  */
/* The above copyright notice and this permission notice shall be included */
/* in all copies or substantial portions of the Software. */
/*  */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR */
/* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, */
/* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL */
/* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER */
/* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER */
/* DEALINGS IN THE SOFTWARE. */
/*  */
/* *************************************************************************** */

module Btn = {
  let json = "JSON"
  let download = "DOWNLOAD"
  let yes = "YES"
  let no = "NO"
  let try_again = "TRY AGAIN"
  let install_and_restart_now = "INSTALL AND RESTART NOW"
  let ill_do_it_later = "I'LL DO IT LATER"
  let metadata = "METADATA"
  let collected = "COLLECTED"
  let gallery = "GALLERY"
  let view_specs = "VIEW SPECS"
  let goto_doc = "See help"
  let goto_settings = "Go to settings"
  let retry_network = "Retry"
  let retry = "RETRY"
  let view_nft = "View NFT"
  let create_or_import_secret = "CREATE OR IMPORT SECRET"
  let buy_tez = "BUY TEZ"
  let verify_accounts = "VERIFY ACCOUNTS"
  let customize_derivation_path = "Customize Derivation Path"
  let ok = "OK"
  let cancel = "CANCEL"
  let continue = "CONTINUE"
  let delete = "DELETE"
  let finish = "FINISH"
  let add = "ADD"
  let send = "SEND"
  let save = "SAVE"
  let done_ = "SIMPLE VIEW"
  let edit = "MANAGEMENT VIEW"
  let go_back = "GO BACK"
  let go_operations = "GO TO OPERATIONS"
  let confirm = "CONFIRM"
  let update = "UPDATE"
  let save_and_register = "SAVE AND REGISTER"
  let import_secret = "IMPORT SECRET WITH RECOVERY PHRASE"
  let create_secret_new = "CREATE NEW SECRET"
  let connect_hardware_wallet = "CONNECT LEDGER"
  let create_account_record_ok = "OK, I'VE RECORDED IT"
  let add_account = "ADD ACCOUNT"
  let add_contact = "ADD CONTACT"
  let add_contract = "REGISTER CONTRACT"
  let advanced_options = "ADVANCED OPTIONS"
  let delegate = "DELEGATE"
  let add_transaction = "ADD TRANSACTION"
  let send_submit = "SUBMIT TRANSACTION"
  let proposal_submit = "PREPARE TRANSACTION"
  let batch_submit = "SUBMIT BATCH"
  let batch_proposal = "PREPARE BATCH"
  let send_cancel = "CANCEL TRANSACTION"
  let delegation_submit = "CONFIRM DELEGATION"
  let delegated = "DELEGATED"
  let delete_token = "Remove from wallet"
  let \"import" = "IMPORT"
  let \"export" = "EXPORT"
  let export_public_key = "EXPORT PUBLIC KEY"
  let export_as_qr_code = "EXPORT AS QR CODE"
  let scan = "SCAN"
  let upgrade = "Download"
  let load_file = "LOAD FROM FILE"
  let csv_format_link = "SEE FILE SPECS"
  let disclaimer_agree = "CONTINUE"
  let deny = "DENY"
  let allow = "ALLOW"
  let reject = "REJECT"
  let close = "CLOSE"
  let add_custom_network = "ADD CUSTOM NETWORK"
  let beacon_connect_dapp = "CONNECT TO DAPP"
  let custom = "CUSTOM"
  let see_list = "SEE LIST"
  let upload_file = "UPLOAD FILE"
  let access_wallet = "ACCESS WALLET"
  let restore_secret = "RESTORE FROM BACKUP"
  let browse_for_folder = "BROWSE FOR FOLDER"
  let setup_later = "Set up later in settings"
  let check_for_updates = "CHECK FOR UPDATES"
  let individual_accounts = "Individual Accounts"
  let multisig_accounts = "Multisig Accounts"
  let token = "Token"
  let multisig = "Multisig"
  let create_new_multisig = "CREATE NEW MULTISIG"
  let add_another_owner = "ADD ANOTHER OWNER"
  let back = "BACK"
  let submit = "SUBMIT"
  let history = "History"
  let pending_operations = "In preparation"
  let sign = "SIGN"
  let sign_anyway = "SIGN ANYWAY"
  let global_batch_add_short = "INSERT INTO BATCH"
  let update_cancel = "CANCEL UPDATE"
}

module Tooltip = {
  let close = "Close"
  let show_qr = "Show QR Code"
  let copy_clipboard = "Copy to Clipboard"
  let addressbook_edit = "Edit Contact"
  let addressbook_delete = "Delete Contact"
  let add_contact = "Add to Contacts"
  let add_contract = "Register contract"
  let unregistered_token_transaction = "This transaction was made using an unregistered token."
  let no_tez_no_delegation = "Delegation requires tez"
  let refresh = "Refresh"
  let sync = "Sync"
  let stop_sync = "Stop sync"
  let open_in_explorer = "Open in explorer"
  let update_delegation = "update delegation"
  let chain_not_connected = "Not connected to a chain"
  let custom_network_edit = "Edit network"
  let custom_network_delete = "Delete network"
  let reject_on_ledger = "Reject on ledger to cancel operation"
  let see_network_info = "See network info"
  let tokenid = "A non-negative number that identifies the asset inside a FA2 contract"
  let add_to_wallet = "Add to wallet"
  let reject_on_provider = s => `Reject on ${s} popup to cancel operation`
}

module Log = {
  let copied_to_clipboard = s => `${s} copied to clipboard`
  let copy_to_clipboard_err = "Copying to Clipboard failed"
  let log_content = "Log content"
  let address = "Address"
  let content = "Content"
  let beacon_sign_payload = "Sign Payload"
}

module Label = {
  let provider_account = "account"
  let tezos_address = "Tezos address"
  let tz_address = "tz address"
  let nft_spec_name = "Name"
  let nft_spec_symbol = "Symbol"
  let nft_spec_contract_address = "Contract Address"
  let nft_spec_token_id = "Token ID"
  let nft_spec_decimals = "Decimals"
  let nft_spec_boolean_amount = "Boolean Amount"
  let nft_spec_creators = "Creators"
  let nft_spec_thumbnail_uri = "Thumbnail URI"
  let nft_spec_artifact_uri = "Artifact URI"
  let nft_spec_display_uri = "Display URI"
  let nft_spec_formats = "Formats"
  let nft_spec_description = "Description"
  let derivation_scheme = "Derivation Scheme"
  let derivation_path = "Derivation Path"
  let accounts = "Accounts"
  let contract_address = "Contract address"
  let advanced_options = "Advanced Options"
  let recovery_phrase_format = "Recovery Phrase Format"
  let account_create_name = "Name"
  let add_contact_name = "Name"
  let add_contact_address = "Address"
  let automatically_download_updates = "Automatically download updates"
  let send_amount = "Amount"
  let send_sender = "Sender Account"
  let send_recipient = "Recipient Account"
  let send_nft = "NFT"
  let password = "Password"
  let fee = "Fee"
  let implicit_reveal_fee = "Implicit Reveal Fee"
  let gas_limit = "Gas Limit"
  let storage_limit = "Storage Limit"
  let confirm_password = "Confirm Password"
  let account_delegate = "Account to delegate"
  let baker = "Baker"
  let add_token_address = "Address"
  let add_token_name = "Name"
  let add_token_symbol = "Symbol"
  let add_token_decimals = "Decimals"
  let add_token_id = "Token ID"
  let summary_subtotal = "Subtotal"
  let summary_total = "Total"
  let summary_total_tez = "Total tez"
  let account_secret = "Root"
  let account_umami = "Umami"
  let token = "Token"
  let token_id = id => "Token ID: " ++ id
  let editions = number => "Editions: " ++ number
  let account_default_path = "Default Path - m/44'/1729'/?'/0'"
  let account_custom_path = "Custom Path"
  let beacon_client_name = "Umami"
  let beacon_account = "Account to connect to dApp"
  let beacon_sign_payload = "Payload to sign"
  let beacon_dapp_pairing = "DApp pairing request"
  let parameters = "Parameters"
  let storage = "Storage"
  let code = "Code"
  let custom_network_name = "Name"
  let custom_network_mezos_url = "Mezos URL"
  let custom_network_node_url = plural => "Node URL" ++ (plural ? "s" : "")
  let file = "File"
  let storage_location = "Storage location"
  let entrypoint = "Entrypoint"
  let parameter = "Parameter"
  let add_contract_name = "Contract name"
  let owners = "Owners"
  let approval_threshold = "Approval threshold:"
  let out_of = (a, b) => `${a} out of ${b}`
  let signing_account = "Signing account"
}

module Input_placeholder = {
  let provider_handler = (provider, handlerKind) => `Type in a ${provider} ${handlerKind}`
  let tez_amount = "0.000000"
  let token_amount = "0"
  let add_accounts_name = "Enter account's name"
  let add_contacts_name = "Enter contact's name"
  let add_contacts_tz = "Enter contact's tz address"
  let add_contact_or_tz = "Paste a tz address or type in a contact's name"
  let add_token_address = "Enter KT1 address of a contract"
  let add_token_name = "e.g. Tezos"
  let add_token_decimals = "e.g. 0"
  let add_token_id = "e.g. 0"
  let enter_new_password = "Enter new password, at least 8 characters"
  let confirm_password = "Confirm your new password"
  let add_token_symbol = "e.g. tez, KLD, ..."
  let enter_derivation_path = "Enter your derivation path"
  let custom_network_name = "e.g. Test Network"
  let search_for_nft = "Search for NFT by name"
  let search_for_contract = "Search by name, symbol or KT address"
  let custom_network_node_url = "e.g. https://rpc.tzbeta.net"
  let custom_network_mezos_url = "e.g. https://api.umamiwallet.com/mainnet"
  let select_backup_path = "Browse to the required folder or enter a path"
  let add_contract_name = "Enter contract name"
  let set_owner = "Paste tz address/Tezos Domains alias"
}

module Form_input_error = {
  let invalid_handler = (provider, handlerKind) => `Invalid ${provider} ${handlerKind}`
  let dp_not_a_dp = "Not a derivation path"
  let dp_more_than_1_wildcard = "Cannot have more than one '?'"
  let dp_missing_wildcard = "Missing '?' or 0"
  let dp_not_tezos = "Not a BIP44 Tezos Path"
  let not_an_int = "This needs to be a number"
  let negative_int = "This needs to be a natural number"
  let expected_decimals = n => `allows at most ${string_of_int(n)} decimals`
  let hardware_wallet_not_ready = "Connection to Tezos ledger application failed. Please make sure the Tezos app is opened and the ledger unlocked."
  let hardware_wallet_plug = "The connection took too long.\nPlease make sure the Ledger is properly plugged in and unlocked"
  let hardware_wallet_check_app = "Public key export failed. Please open the Tezos Wallet app and retry."
  let hardware_wallet_timeout = "Timeout while connecting to Ledger.\nPlug and unlock your Ledger then retry"
  let hardware_wallet_signer_inconsistent = "Inconsistency between signing intent and key"
  let empty_transaction = "Transaction is empty."
  let branch_refused_error = "Please retry. An error came up while communicating with the node"
  let invalid_key_hash = "Invalid contract address"
  let key_already_registered = a => `Address already registered under: ${a}`
  let name_already_registered = "Name already registered"
  let mandatory = "This input is mandatory"
  let int = "must be an integer"
  let float = "must be a float"
  let change_baker = "must be a different baker than the current one"
  let wrong_password = "Wrong password"
  let password_length = "Password length must be at least 8 characters"
  let unregistered_delegate = "This key is not registered as a baker"
  let bad_pkh = "Not a valid key"
  let invalid_contract = "The recipient is not a key or an alias"
  let balance_too_low = "Balance is too low"
  let account_balance_empty = "Account is empty"
  let confirm_password = "It must be the same password"
  let derivation_path_error = "Invalid derivation path"
  let name_already_taken = a => `${a} is already taken`
  let api_not_available = "API not available"
  let node_not_available = "Node not available"
  let different_chains = "API and Node are not running on the same network"
  let not_a_token_contract = "Not a standard token contract"
  let no_metadata = pkh => {
    let s = pkh->Option.mapWithDefault("", s => `: ${s}`)
    `No metadata was found for the contract${s}.`
  }
  let no_token_metadata = pkh => {
    let s = pkh->Option.mapWithDefault("", s => `: ${s}`)
    `No token metadata was found for the contract${s}.`
  }
  let token_id_not_found = x =>
    switch x {
    | None => "Requested token id was not found"
    | Some(pkh, tokenId) => `Token id ${string_of_int(tokenId)} not found for contract ${pkh}`
    }
  let illformed_token_metadata = (pkh, tokenId, field) =>
    `Field ${field} for token id ${string_of_int(tokenId)} illformed for contract ${pkh}`
  let not_a_multisig_contract = "Contract not supported by Umami"
  let permissions_error = "Unable to propose a multisig transaction: none of your wallet accounts is a co-owner ot the selected contract."
}

module Title = {
  let custom_auth_sign_in_up = "Sign up or Sign in with"
  let custom_auth_waiting_auth = "Waiting for authentification"
  let custom_auth_failed = "Authentification failed"
  let custom_auth_success = "Successful authentification"
  let provider_authentification = pr => `${pr} authentification`
  let nft_specs = "View Specs"
  let error_logs = "Logs"
  let checking_for_updates = "Checking for Updates"
  let install_update = "Install Update"
  let update_available = "Update Available"
  let wallet_up_to_date = s => `Your wallet is up to date (v${s})`
  let secret_create = "Create New Secret"
  let derive_account = "Add Account"
  let account_update = "Edit Account"
  let import_account = "Import Secret"
  let hardware_wallet_connect = "Connect Ledger"
  let hardware_wallet_confirm = "Confirm on Ledger"
  let hardware_wallet_op_confirm = "Ledger found, please confirm the operation on the device"
  let hardware_wallet_confirmed = "Confirmed on Ledger"
  let hardware_wallet_op_confirmed = "Transaction confirmed on Ledger"
  let hardware_wallet_search = "Searching for Ledger"
  let hardware_wallet_error_unknown = "Ledger unknown error"
  let hardware_wallet_not_found = "Ledger not found"
  let hardware_wallet_not_ready = "Ledger not ready"
  let hardware_wallet_denied = "Operation rejected on Ledger"
  let hardware_wallet_error_app = "Tezos application not started or export rejected"
  let hardware_wallet_check_follow = "Follow the checklist"
  let hardware_confirm_pkh = "Please confirm public key export on your Ledger."
  let secret_update = "Edit Secret"
  let add_contact = "Add Contact"
  let update_contact = "Edit Contact"
  let contract_details = "Contract Details"
  let add_token = "Register Token"
  let add_multisig = "Register Contract"
  let edit_metadata = "Token Metadata"
  let export_to_mobile = "Export to Umami Mobile"
  let show_recovery_phrase = "Show Recovery Phrase"
  let account_create_password = "Set a password to secure your wallet"
  let account_enter_password = "Please enter password to confirm"
  let account_create_verify_phrase = "Verify your recovery phrase"
  let account_derivation_path = "Derivation Path"
  let import_account_enter_phrase = "Enter your recovery phrase"
  let operation_submitted = "Operation Submitted"
  let sender_account = "Sender Account"
  let baker_account = "Baker"
  let withdraw_baker = "Withdraw Baker"
  let confirm_delegate = "Confirm Delegate"
  let confirm_batch = "Confirm"
  let confirm_multisig_origination = "Confirm"
  let confirm_operation_approval = "Confirm"
  let confirm_operation_execution = "Confirm"
  let confirm_operation_approval_and_execution = "Confirm"
  let delegate = "Delegate"
  let delegate_update = "Change Baker"
  let delegate_delete = "End Delegation"
  let delete_contact = "Delete Contact?"
  let delete_beacon_peer = "Delete Peer"
  let delete_beacon_peers = "Delete Peers"
  let delete_beacon_permission = "Delete Permission"
  let delete_beacon_permissions = "Delete Permissions"
  let confirm_cancel = "Are you sure you want to cancel?"
  let send = "Send"
  let confirmation = "Confirmation"
  let send_many_transactions = "Send one or many transactions"
  let delete_account = "Delete Account?"
  let delete_batch = "Delete this batch?"
  let delete_contract = "Delete contract?"
  let delete_secret = "Delete Secret?"
  let scan = "Scan"
  let disclaimer = "User Agreement"
  let beacon_connection_request = "Connection Request"
  let beacon_sign_request = "Sign"
  let beacon_pairing = "Connect to DApp with pairing request"
  let beacon_pairing_qr = "Connect to DApp with QR code"
  let beacon_scan_qr = "Scan QR code from dApp"
  let add_custom_network = "Add Custom Network"
  let update_custom_network = "Edit Custom Network"
  let delete_custom_network = "Delete Network?"
  let beacon_error = "Beacon Error"
  let accounts = "Accounts"
  let global_batch = "Batch"
  let global_batch_empty = "Your batch is currently empty"
  let operations = "Operations"
  let addressbook = "Address Book"
  let delegations = "Delegations"
  let contracts = "Contracts"
  let settings = "Settings"
  let collected = "Collected"
  let gallery = "Gallery"
  let notice = "Disclaimer Notice"
  let buy_tez = "Buy tez"
  let wert_recipient = "Please select the recipient account"
  let multisig_sender = "Please select the sender account"
  let added_to_wallet = "Added to wallet"
  let held = "Held"
  let restore_account = "Restore from Backup"
  let select_backup_file = "Select a JSON backup file and enter the password you used to encrypt it."
  let account_backup_path = "Select a storage location for your backup"
  let details = "Details"
  let help = "Help"
  let scan_with_mobile_to_import = "Scan with Umami Mobile to import"
  let create_new_multisig = "Create New Multisig"
  let name_contract = "Name contract"
  let set_owners_and_threshold = "Set owners & threshold"
  let review_and_submit = "Review and Submit"
  let contract_name = "Contract name"
  let owners = s => `Owners (${s})`
  let approval_threshold = "Approval threshold"
  let approval_threshold_reached = "Signature threshold reached"
  let unrecognized_operation = "Unrecognized Operation"
  let propose_batch = "Prepare Batch"
}

module Expl = {
  let custom_auth_success = "Please click on confirm to submit operation"
  let custom_auth_sign = s => `Please log into ${s} to sign the operation`
  let custom_auth_confirm_operation = "Please validate the details of the transaction and confirm it by signing up with your social account."
  let provider_authentification = s => `Please log into ${s} to import your key`
  let nft_empty_state = "Umami should automatically discover any NFT you possess."
  let network_disconnect = "The Tezos network is currently unreachable. Your internet connection might be unstable. If it is not the case, you should check your configuration and update it by following the documentation"
  let hardware_wallet_confirm_operation = "Please validate the details of the transaction and press Confirm to sign it on your Ledger."
  let hardware_wallet_advopt = "Umami wallet supports three derivation schemes as well as custom derivation path to select new addresses. You may also use the default derivation scheme & path by leaving the preselected values."
  let hardware_wallet_check_complete_steps = "Complete the steps in order to connect."
  let hardware_wallet_check_firmware = "Make sure your Ledger has the latest firmware version."
  let hardware_wallet_check_app = "Install and open the Tezos Wallet app on your Ledger."
  let hardware_wallet_check_plug = "Plug your Ledger into your computer using a USB cable."
  let hardware_wallet_check_unlock = "Unlock your Ledger."
  let hardware_wallet_check_click = "Click the EXPORT PUBLIC KEY button below and confirm the action on your Ledger."
  let hardware_wallet_confirmed = "Public key export confirmed"
  let hardware_wallet_op_confirmed = "Operation signed and sent to the network."
  let hardware_wallet_confirm = "Ledger found. Please confirm public key export."
  let hardware_wallet_op_confirm = "Verify the operation details displayed on your Ledger before you sign."
  let hardware_wallet_denied = "The operation has been rejected from the Ledger device"
  let hardware_wallet_search = "Please make sure to unlock your Ledger and open the Tezos Wallet app."
  let scan = "A scan will check the Tezos chain for other accounts from your secrets that may have been revealed outside of this wallet."
  let secret_create_record_recovery = "Please record the following 24 words in sequence in order to restore it in the future. Ensure to back it up, keeping it securely offline."
  let secret_create_record_verify = "We will now verify that you've properly recorded your recovery phrase. To demonstrate this, please type in the word that corresponds to each sequence number."
  let secret_create_password_not_recorded = "Please note that this password is not recorded anywhere and only applies to this machine."
  let secret_select_derivation_path = "Umami wallet supports custom derivation path to select new addresses. You may also select the default derivation path and use the default key."
  let import_secret_enter_phrase = "Please fill in the recovery phrase in sequence."
  let confirm_operation = "Please validate the details of the transaction and enter password to confirm"
  let global_batch = "Please validate the details of the batch and sign the transaction."
  let fill_batch = "Add elements via the send menu or load a CSV file"
  let delete_batch = "You are about to delete all the transactions included in the batch."
  let operation = "The operation will be processed and confirmed, you can see its progress in the Operations section."
  let beacon_dapp = "would like to connect to your wallet"
  let beacon_dapp_request = `This site is requesting access to view your account's address.\\nAlways make sure you trust the sites you interact with.`
  let beacon_operation = "Requests Operations"
  let beacon_dapp_sign = "requests your signature"
  let custom_network = "Please specify the parameters of the network you want to connect to."
  let external_service = "Notice: you are using Wert, which is an external service to Umami."
  let secret_select_backup_path = `If you ever need to restore your wallet, you can do so with this backup file and your password.`
  let no_multisig_contract = "No contract on the current chain"
  let name_contract = "Please give a name to the contract you are about to create. This name is only stored locally on your computer and never shared with Umami or any third-party."
  let set_owners_and_threshold = "You are about to create a new multisig contract that has one or more owners. Please select the addresses that have permission to submit and approve transactions."
  let name_multisig = `Please give a name to the contract you are about to create. This name is only stored locally on your computer and never shared with Umami or any third-party.`
  let set_multisig_owners = `You are about to create a new multisig contract that has one or more owners. Please select the addresses that have permission to submit and approve transactions.`
  let set_multisig_threshold = "Set the minimum number of required approvals before any action can be executed."
  let out_of = s => `out of ${s}`
  let review_multisig = `Please review the details of the multisig contract you are about to create. You can edit them before submitting by clicking the back button. You will receive an approval request from your wallet. Please confirm it to continue.`
  let approval_threshold_reached = "This operation has already gathered the required number of signatures to be submitted. You signature is therefore no longer necessary."
  let signing_account = "Select signing account."
  let signing_account_multisig = s =>
    `A multisig (${s}) was selected. You need to propose this operation to the co-owners of the selected multisig and then approve it in order to finally execute it. Please select which account will propose.`
  let unrecognized_operation = "Umami does not support this type of operation. Please review the details of the operation before signing it."
}

module Menu = {
  let app_menu_support = "Support"
  let app_menu_help = "Help"
  let app_menu_new_version = "Check for new versions"
  let app_menu_website = "Website"
  let delegate_edit = "Change Baker"
  let delegate_delete = "End Delegation"
  let batch_edit = "Edit Transfer"
  let batch_delete = "Delete Transfer"
  let delete_account = "Delete"
  let delete_secret = "Delete"
  let delete_contract = "Delete"
  let edit = "Edit"
  let scan = "Scan"
  let show = "Show"
  let view_in_explorer = "View in Explorer"
  let see_metadata = "See Metadata"
  let see_details = "See details"
}

module Settings = {
  let theme_title = "THEME"
  let theme_system = "Default to system"
  let theme_dark = "Dark"
  let theme_light = "Light"
  let confirmations_title = "VERIFICATION"
  let confirmations_label = "Number of confirmations (blocks)"
  let confirmations_saved = "Number of confirmations saved"
  let chain_title = `CHAIN/NETWORK`
  let danger_title = "DANGER ZONE"
  let danger_reset_section = "Reset Settings"
  let danger_reset_text = `This will remove or restore custom settings to default values.`
  let danger_reset_button = "RESET"
  let danger_reset_confirm_title = "Reset Settings"
  let danger_reset_confirm_text = `This will remove or reset all customized settings to their defaults. Personnal data -including saved contacts, password and accounts- won't be affected.`
  let danger_reset_confirm_button = "RESET SETTINGS"
  let danger_offboard_section = "Offboard Wallet"
  let danger_offboard_text = `Offboarding will permanently delete any data from this computer. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the backup phrase.`
  let danger_offboard_button = "OFFBOARD"
  let danger_offboard_form_title = "Offboard Wallet"
  let danger_offboard_form_text = `Offboarding will permanently delete any data from this computer. Please acknowledge that you have read and understood the disclaimer, then enter « wasabi » to confirm. The accounts are still available to be imported in the future ; in order to regain access to your accounts, please make sure that you keep the recovery phrase.`
  let danger_offboard_form_checkbox_label = `I have read the warning and I am certain I want to delete my private keys locally. I also made sure to keep my recovery phrase.`
  let danger_offboard_form_input_placeholder = "Enter code word to confirm"
  let danger_offboard_form_input_error = "Not the correct confirm code word"
  let beacon_title = "DAPPS"
  let beacon_peers_section = "Peers"
  let beacon_peers_name = "NAME"
  let beacon_peers_relay = "RELAY SERVER"
  let beacon_peers_remove_all = "REMOVE ALL"
  let beacon_empty_peers = "No Peer"
  let beacon_permissions_section = "Permissions"
  let beacon_permissions_dapp = "DAPP"
  let beacon_permissions_account = "CONNECTED ACCOUNT"
  let beacon_permissions_scopes = "SCOPES"
  let beacon_permissions_network = "NETWORK"
  let beacon_permissions_remove_all = "REMOVE ALL"
  let beacon_empty_permissions = "No Permission"
  let backup_title = "WALLET BACKUP"
  let backup_text = "Create and save a backup file for the entire wallet"
  let backup_path_saved = "Backup file path saved"
  let app_update = "APP UPDATE"
  let about_ELEMENT = onPress => <>
    {"Powered by "->React.string}
    <ReactNative.Pressable onPress> {_ => "TzKT"->React.string} </ReactNative.Pressable>
    {" API"->React.string}
  </>
}

module Help = {
  let subtitle = "Find the resources you need to get the most out of Umami Wallet."
  let articles_descr = "Don't know how to use a feature? The answer might be in our articles."
  let articles_text = "BROWSE ARTICLES"
  let faq_descr = "Got a question? It may already be answered in here."
  let faq_text = "CONSULT FAQ"
  let support_descr = "Need further assistance? We're happy to help!"
  let support_text = "EMAIL SUPPORT"
  let tos_descr = "Want to learn about what you can expect from Umami wallet?"
  let tos_text = "READ TERMS OF SERVICE"
}

module Network = {
  let api_not_available = "The API is not available"
  let api_version_rpc_error = err => `The API \`/version\` RPC parsing returned an error: ${err}`
  let api_version_format_error = a =>
    `The API \`/version\` RPC parsing returned an unknown version format: ${a}`
  let api_monitor_rpc_error = a => `The API \`/monitor/blocks\` RPC parsing returned an error: ${a}`
  let node_not_available = "The node is not available"
  let node_version_rpc_error = a =>
    `Node \`/chains/main/chain_id\` RPC parsing returned an error: ${a}`
  let chain_inconsistency = (a, b) =>
    `The API and Node are not running on the same network.\nAPI runs on chain \`${a}\` and the node on the chain \`${b}\``
  let unknown_chain_id = a =>
    `Network ${a} is not supported, your operation cannot be viewed in an explorer.`
  let unknown_explorer = a => `No explorer is known on network ${a}`
  let api_not_supported = a => `The API ${a} is not supported by this version of Umami.`
  let api_and_node_not_available = "API and Node are both not available"
  ()
}

module Taquito = {
  let not_an_account = "Not a tz address"
  let not_a_contract = "Not a contract address"
  let no_prefix_matched = "Unknown address prefix"
  let api_error = n => `API error: received ${string_of_int(n)}`
  let invalid_checksum = "Invalid checksum"
  let invalid_length = "Invalid length"
}

module Wallet = {
  let key_not_found = "Key not found"
  let key_bad_format = s => `Can't readkey, bad format: ${s}`
  let invalid_path_size = s => `Path ${s} is not valid: it must be of at least of two indexes`
  let invalid_index = (index, value) =>
    `Value ${value} at index ${string_of_int(index)} is invalid for a derivation path`
  let invalid_scheme = scheme => `${scheme} is not a valid scheme for an encoded Public key`
  let invalid_encoding = enc => `${enc} is not a valid an encoded Public key`
  let invalid_ledger = s => `The Ledger connected has base key ${s}, which is not the one expected.`
  ()
}

module Errors = {
  let download_error_status = n => `Request failed with status code ${n}`
  let download_error = "Error during file download"
  let error_while_updating = "There's been an error while updating."
  let error_while_checking_updates = "Unable to check for updates."
  let unable_to_download = "Unable to download"
  let invalid_estimation_results = "Invalid estimation results"
  let request_to_node_failed = "Request to node failed"
  let every_balances_fail = "Every balances fail to load"
  let balance_not_found = "A balance was not found"
  let network_unreachable = "Tezos network unreachable."
  let no_valid_endpoint = "No valid endpoint found among public nodes"
  let beacon_cant_handle = "Cannot handle this operation"
  let customauth_unable_to_retrieve_handle = "Unable to retrieve user handle"
  let customauth_popup_closed = "User closed popup during authentification"
  let customauth_handle_mismatch = (a, b) => `Signing handle mismatch: ${a} & ${b}`
  let invalid_provider = s => `${s} is not a valid provider`
  let deeplinking_not_connected = "Deep linking required but not connected"
  let stream = "Acquiering media failed"
  let decryption = "Storage decryption failed"
  let encryption = "Storage encryption failed"
  let key_derivation = "Key derivation failed"
  let illformed_token_contract = "Illformed Token Contract"
  let cannot_read_token = s => `Cannot read token amount: ${s}`
  let unknown_kind = k => `Internal error: unknown kind \`${k}\` for token contract`
  let invalid_operation_type = "Invalid operation type!"
  let unhandled_error = e => `Unhandled error ${e}`
  let no_secret_found = "No secrets found"
  let secret_not_found = i => `Secret at index ${string_of_int(i)} not found!`
  let cannot_update_secret = i => `Can't update secret at index ${string_of_int(i)}!`
  let recovery_phrase_not_found = i => `Recovery phrase at index ${string_of_int(i)} not found!`
  let beacon_operation_not_supported = "Beacon operation not supported"
  let beacon_request_network_missmatch = "Beacon request network not supported"
  let beacon_client_not_created = "Beacon client not created"
  let video_stream_access_denied = "Unable to access video stream\n(please make sure you have a webcam enabled)"
  let incorrect_number_of_words = "Mnemonic must have 12, 15 or 24 words."
  let unknown_bip39_word = (w, i) =>
    `Word ${string_of_int(i)} ('${w}') is not a valid BIP39 word, please refer to the standard.`
  let secret_already_imported = "Secret already imported"
  let json_parsing_error = _ => "JSON parsing error"
  let local_storage_key_not_found = k => `Internal error: key \`${k}\` not found`
  let version_format = v => `Internal error: invalid version format \`${v}\``
  let storage_migration_failed = v => `Internal error: storage migration failed at version ${v}`
  let unknown_network = c => `No public network exists for chain ${c}`
  let script_parsing = e => {
    let e = e->Option.mapDefault("", m => " with message: " ++ m)
    `Error when parsing script${e}`
  }
  let micheline_parsing = e => {
    let e = e->Option.mapDefault("", m => " with message: " ++ m)
    `Error when parsing Micheline${e}`
  }
  let fetching_storage = (pkh, e) => `Error while fetching storage of ${pkh}: ${e}`
  let unknown_version = (current, expected) =>
    `Unknown version ${current}, while ${expected} expected`
  let version_not_in_bound = (lowest, highest, version) =>
    `Version ${version} is not in the range of ${lowest} and ${highest}`
  let unknown_backup_version = v =>
    `Version ${v} of backup file cannot be imported in the current version of Umami.`
  let cannot_parse_version = err => `Invalid backup file:\n${err}`
  let gas_exhausted = "Gas exhausted for the operation, please put a higher limit"
  let storage_exhausted = "Storage exhausted for the operation, please put a higher limit"
  let gas_exhausted_above_limit = "Gas exhausted and above protocol limit per operation"
  let storage_exhausted_above_limit = "Storage exhausted and above protocol limit per operation"
  let not_a_number = v => `${v} is not a valid number`
  let negative_number = v => `${v} is negative`
  let unexpected_decimals = v => `${v} does not accept decimals`
  let expected_decimals = (v, d) => `${v} does not accept more that ${string_of_int(d)} decimals`
  let fetch404 = "Error 404 - Not Found"
  let reload_btn = "RELOAD PAGE"
}

module Csv = {
  let cannot_parse_number = (row, col) =>
    `Value at row ${string_of_int(row)} column ${string_of_int(col)} is not a number`
  let cannot_parse_boolean = (row, col) =>
    `Value at row ${string_of_int(row)} column ${string_of_int(col)} is not a boolean`
  let cannot_parse_custom_value = (err, row, col) =>
    `Value at row ${string_of_int(row)} column ${string_of_int(col)} is not valid:\n${err}`
  let cannot_parse_row = row =>
    `Row ${string_of_int(row)} is not valid, some columns are probably missing`
  let cannot_parse_csv = "CSV is not valid"
  let no_rows = "CSV is empty"
  let cannot_parse_token_amount = (v, row, col) => {
    let v = ReBigNumber.toString(v)
    `Value ${v} at row ${string_of_int(row)} column ${string_of_int(
        col,
      )} is not a valid token amount`
  }
  let cannot_parse_tez_amount = (v, row, col) => {
    let v = ReBigNumber.toString(v)
    `Value ${v} at row ${string_of_int(row)} column ${string_of_int(col)} is not a valid tez amount`
  }
  let unknown_token = (pkh, id) => {
    let id = id->Option.mapWithDefault("", n => ` and tokenId ${string_of_int(n)}`)
    `Unknown token ${pkh}${id}`
  }
  let cannot_parse_address = (a, reason) => `${a} in not a valid address: ${reason}.`
  let fa1_2_invalid_token_id = pkh => `Contract ${pkh} is an FA1.2 token, it cannot have a token id`
  let fa2_invalid_token_id = pkh => `Contract ${pkh} is an FA2 token, it must have a token id`
}

module Disclaimer = {
  let last_updated = date => `Last updated ${date}.`
  let please_sign = "In order to use Umami, \nyou must agree to the terms."
  let agreement_checkbox = "Check here to indicate that you have read and agree to the terms of the User Agreement"
}

let support_mailto_error_subject = "Support: Error log received on Umami"
let support_mailto_error_body = "The following error happened in Umami:"
let download_question = "Do you want to download the latest update?"
let restart_to_install_question = "Do you want to restart the app to install now? Please note that the update will be applied next time you restart Umami."
let unknown_operation = "Unknown"
let unrecognized_operation = "Unrecognized Operation"
let error404 = "404 - Route Not Found :("
let no_balance_amount = "---- "
let logs_no_recent = "No Recent Message"
let logs_clearall = "CLEAR ALL"
let amount = (a, b) => `${a} ${b}`
let tezos = "Tez"
let tez = "tez"
let you_dont_have_nft = "You don't have any NFT yet"
let tez_amount = a => `${a} ${tez}`
let tez_amount_ELEMENT = a => <> {a} <span> {` ${tez}`->React.string} </span> </>
let tez_op_amount = (op, a) => `${op} ${tez_amount(a)}`
let account = "Account"
let operation_reveal = "Reveal"
let operation_transaction = "Transaction"
let operation_contract_call = "Call"
let operation_origination = "Origination"
let operation_delegation = "Delegation"
let operation_batch = "Batch"
let operation_token_batch = "Token batch"
let operation_column_type = "TYPE"
let operation_column_amount = "AMOUNT"
let operation_column_recipient = "RECIPIENT"
let operation_column_timestamp = "TIMESTAMP"
let operation_column_fee = "FEE"
let operation_column_sender = "SENDER"
let operation_column_status = "STATUS"
let operation_column_ID = "ID"
let operation_column_signatures = "SIGNATURES"
let global_batch_column_type = "TYPE"
let global_batch_subject = "SUBJECT"
let global_batch_recipient = "RECIPIENT"
let global_batch_fee = "FEE"
let global_batch_delete_all = "DELETE ALL"
let global_batch_add = "INSERT TRANSACTION INTO BATCH"
let global_batch_add_multisig_exectute = "ADD TO BATCH"
let state_mempool = "Mempool"
let state_levels = (a, b) => `${string_of_int(a)}/${string_of_int(b)} blocks`
let state_confirmed = "Confirmed"
let stepof = (a, b) => `Step ${string_of_int(a)} of ${string_of_int(b)}`
let optional_stepof = (a, b) => `Step ${string_of_int(a)} of ${string_of_int(b)} (optional)`
let account_create_record_recovery = "Record your recovery phrase"
let contact_added = "Contact Added"
let contact_updated = "Contact Updated"
let multisig_originated = "Multisig Originated"
let multisig_updated = "Multisig edited"
let contract_removed_from_cache = "Contract removed from cache"
let contact_deleted = "Contact Deleted"
let account_created = "Account Created"
let account_updated = "Account Updated"
let secret_updated = "Secret Updated"
let secret_deleted = "Secret Deleted"
let token_contract = s => `${s} Token Contract`
let token_created = "Token added to wallet"
let token_deleted = "Token removed from wallet"
let operation_hash = "Operation Hash"
let navbar_accounts = "ACCOUNTS"
let navbar_nft = "NFT"
let navbar_operations = "OPERATIONS"
let navbar_addressbook = "ADDRESS BOOK"
let navbar_delegations = "DELEGATIONS"
let navbar_contracts = "CONTRACTS"
let navbar_settings = "SETTINGS"
let navbar_logs = "LOGS"
let navbar_global_batch = "BATCH"
let navbar_help = "HELP"
let delegate_column_account = "ACCOUNT"
let delegate_column_initial_balance = "INITIAL BALANCE"
let delegate_column_current_balance = "CURRENT BALANCE"
let delegate_column_duration = "DURATION"
let delegate_column_last_reward = "LAST REWARD"
let delegate_column_baker = "BAKER"
let token_column_standard = "STANDARD"
let token_column_name = "NAME"
let token_column_symbol = "SYMBOL"
let token_column_address = "ADDRESS"
let token_column_tokenid = "TOKEN ID"
let na = "N/A"
let empty_held_token = "No token held and not registered on the current chain"
let empty_delegations = "No Delegation"
let empty_operations = "No Operation"
let empty_pending_operations = "No pending operation"
let empty_address_book = "No Contact"
let add_token_format_contract_sentence = "Please enter the address of a deployed token contract for which you would like to view balances as well as to perform operations."
let add_token_contract_metadata_fa1_2 = "Please specify the name, symbol, and decimals of a token contract for which you would like to view balances as well as to perform operations. Umami will prefill the fields if any metadata is available."
let add_token_contract_tokenid_fa2 = "Please specify the token ID of the token you would like to perform operations on. Umami will prefill the fields if any metadata is available."
let add_token_contract_metadata_fa2 = "Please specify the token ID of the token you would like to perform operations on. Umami will prefill the fields if any metadata is available"
let add_multisig_contract_sentence = "Please enter the address of a deployed multisig contract."
let delegation_removal = "Delegation Removal"
let error_check_contract = "Address is not a valid token contract"
let error_register_not_fungible = "Cannot register an NFT as a fungible token"
let error_register_not_non_fungible = "Cannot register a fungible token as an NFT"
let words = n => `${string_of_int(n)} words`
let upgrade_notice = "We recommend you upgrade your version of Umami."
let custom_network_created = "Network created"
let custom_network_updated = "Network updated"
let custom_network_deleted = "Network deleted"
let hw = "H/W"
let email = "email"
let username = "username"
let percent_complete = "% complete"
let update_available = "An update is available"
let downloading_update = s => `Downloading update (${s}%)`
let download_complete = "Download complete"
let a_of_b = (a, b) => `${a} of ${b}`
let approved_a_of_b = (a, b) => `Approved (${a} of ${b})`
