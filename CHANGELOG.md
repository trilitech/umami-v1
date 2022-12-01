# umami's CHANGELOG

## Unreleased changes

- Use ImageProxy for all NFTs
- Change support email address

## Beta (0.8.5)

- Add native Limanet support
- Drop native Jakartanet support
- Redesign tez amount display
- Added custom UmamiInstallationHash header field
- Redesign Settings, Help & Logs in Menu bar
- Converted project to rescript
- Fix ledger permission issue with the snap package version

## Beta (0.8.4)

- Upgrade Electron to version 20
- Default confirmation block count set to 2
- Fix close button in toast messages
- Fix wrong account selection while processing beacon request

## Beta (0.8.3)

- Add native Kathmandunet support
- Fix auto update download and notification feature (i.e. is user
  activated the option, the release after this one will automatically
  be downloaded and user will be prompted to install)
- Import Galleon backup
- Export recovery as QR code (for mobile import)
- Fix scrolling issue on small screens

## Beta (0.8.2)

- Fix a crash when restoring the previously closed umami window on
  macOS
- Fix Ghostnet decoding issue with tokens

## Beta (0.8.1)

- Beacon bugfix
- Micheline expression decoding for Beacon sign requests
- NFT bugfix
- Various bug fixes

## Beta (0.8.0)

- Sign in/Sign up and create accounts using social networks (Torus
  CustomAuth integration)
- Exchange quick access (Wert integration)
- Beacon bugfix
- Balance fetching optimizations
- Support buttons on error logs on the log page to send us email
  reports
- Remove hangzhounet
- Various bug fixes

## Beta (0.7.3)

- Native support for test-net Ithacanet
- Application now auto-updates
- Re-style toasts
- Enable mnemonic pasting
- Allow KT addresses in transaction batching
- Old baker appearing in End delegation dialog
- Add "remove all" button to Dapps peers and permssions

## Beta (0.7.0)

- New operation batching feature
- Improve Dapps compatibility
- CSV: add contracts call and KT1 as recipients of transactions (see
  full spec)
- Various bug fixes

## Beta (0.6.4)

- Keystore backup: select a file to backup your secret key (preferably
  in a cloud directory)
- Allow origination on DApps (bug fix for tzprofiles)
- Add type icons on accounts
- Better handling of fungible tokens discovery
- Bug fixes on NFTs
- Bug fixes on Beacon

## Beta (0.6.1)

- View metadata of any NFT
- Show number possessed of each NFTs
- Fixes NFTs not displayed correctly
- Fixes FA2 fungible transfers and balances not displayed correctly
- Fixes Beacon issues on MacOS
- Removed Granadanet endpoint
- Known issues: Missing app icon for deb and rpm packages

## Beta (0.6.0)

- Add support for NFTs
- Add support for FA2 fungible tokens
- Add Hangzhounet testnet as embedded network
- Uses Taquito v11.0.1
- Uses Beacon SDK v2.3.7
- Known issues: Missing app icon for deb and rpm packages

## Beta (0.5.5)

- Use new backend API

## Beta (0.5.4)

- Use several nodes by default
- Use fallback if node is unreachable
- Better error display when network is down
- Improve advanced options (fee, etc), display them when using dapps
  (beacon)
- Various aesthetic improvements
- Various small fixes
- Avoid spamming logs
- fee computation in presence of reveal
- Known issue: Missing app icon for deb and rpm packages

## Beta (0.5.3)

- Fix issue due to Let's encrypt root certificate being
  expired. Should solve connexion issues with public nodes and
  injections.
- Known issue: Missing app icon for deb and rpm packages

## Beta (0.5.2)

- Support for token metadata
- Support for token decimals (manual input if doesn't exists)
- wrong passwords no longer show an empty error
- Cmd-Q on MacOs actually quits Umami
- Improve error handling on derivation path leading to crash
- Known issue: Missing app icon for deb and rpm packages

## Beta (0.5.1)

- Show information on default selected network on Mnemonic:
  - improve speed, experience and stability of secret importation
  - add support all standard mnemonic: 12/15/18/21/24
- Support for FA1.2 tokens in operation list
- Scan: speed improvements
- Better error display: widely improving the display of error messages
- Better snapcraft (linux) binary generation
- Fix high slowness issues during signing (during transfers/initial
  key import etc)
- minor bug fixes

## Beta (0.5.0)

- Ledger support
- Better errors messages with Beacon QR pairing.
- Remove Florencenet

## Beta (0.4.1)

- Beacon through QR code/Copy from clipboard support
- Custom network manager

## Beta (0.4.0)

- Beacon support
- Operations pagination
- Allow the create of new secrets after onboarding
- Bug fixes and internal rewriting for future improvements.
- Linux distribution: move from AppImage to snap

## Beta (0.3.9)

- Add Logs section
- Add EULA
- Onboarding: fix word number missing when verifying the mnemonic
- Add support for Granadanet chain

## Beta (0.3.8)

- Batch diversity (tez + any tokens), with CSV support.
- Added a "Clear" button on addresses inputs.
- Comments in CSV.
- Password error shown besides the input field instead as a toast.
- Fees between simulation and injection were not the same (+100 mutez
  for each transaction).
- Refactoring of some errors handling.
- Improved handling of form inputs through types.
- Improved errors handling when scanning derivative accounts.
- Refactoring of transactions handling

## Beta (0.3.7)

- Batchs from CSV
- Testnet: use Florencenet by default
- Use API 2.0.0
- Fix bug with selected recipient address were not propagated
  correctly in the summary

## Beta (0.3.6)

- Filling Mnemonics: hitting tab should auto-completes current input
- Improve recipient selector in Send form
- Network information shown in header (Mainnet/Testnet)
- Fix a bug where deleting a secret doesn't delete related accounts
- Remove Gas-limit and Storage limit in Send form when sending tez

## Beta (0.3.5)

- Ability to import either 12, 15, 24 word recovery phrases
- Initial window now has a bigger size
- Int overflow bug fixed on tokens amounts
- Mnemonics forms are now scrollable to support smaller screens
- Dropdown menus direction is reversed if close to the bottom of the
  screen
- Replace Electron menus by Umami menus
- Dropdown for contact selector drops a list on focus even when text
  is empty
- Delegate button only appears on tez view
- Wrong password on deriving account produce better errors
- Submitting transaction with empty password is not allowed anymore
- Derivation path with no `?` are now supported
- various forms UI fixes

## Beta (0.3.4)

- Integrate Taquito as node api
- HD Wallet features
- Use SDK for key managment
- Add contact from operation list
- Use tooltips for UI/UX improvement
- Check API and node version consistency
- Check API version and warns when inconsistent
- Explorer link is network-dependant
- Clear LocalStorage when offboarding
- Disable Delegate when account has zero tez
- Show implicit reveal fees
- Network switcher (mainnet/testnet)
- bug fixes and quality of life improvements
