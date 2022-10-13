open ReactNative

let styles = {
  open Style
  StyleSheet.create({
    "rawAddressContainer": style(~display=#flex, ~flexDirection=#row, ~alignItems=#center, ()),
  })
}

@module("uuid") external genUuid: unit => string = "v4"

module AddContactButton = {
  @react.component
  let make = (~address: PublicKeyHash.t) => {
    let (visibleModal, openAction, closeAction) = ModalAction.useModalActionState()

    let tooltip = ("add_contact_from_op" ++ genUuid(), I18n.Tooltip.add_contact)

    let onPress = _e => openAction()

    <>
      <IconButton icon=Icons.AddContact.build onPress tooltip />
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ContactFormView initAddress=address action=Create closeAction />
      </ModalAction>
    </>
  }
}

// Formating address here because css

let rawUnknownAddress = (address: PublicKeyHash.t) =>
  <View style={styles["rawAddressContainer"]}>
    <Typography.Address numberOfLines=1>
      <AccountElements.ShrinkedAddress address />
    </Typography.Address>
    <AddContactButton address />
  </View>
let getContactOrRaw = (aliases, tokens, address) =>
  address
  ->AliasHelpers.getContractAliasFromAddress(aliases, tokens)
  ->Option.mapWithDefault(rawUnknownAddress(address), alias =>
    <Typography.Body1 numberOfLines=1> {alias->React.string} </Typography.Body1>
  )
