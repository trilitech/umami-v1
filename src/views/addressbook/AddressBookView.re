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

open ReactNative;

module AddContactButton = {
  let styles =
    Style.(
      StyleSheet.create({
        "button":
          style(
            ~alignSelf=`flexStart,
            ~marginLeft=(-6.)->dp,
            ~marginBottom=10.->dp,
            (),
          ),
      })
    );

  [@react.component]
  let make = () => {
    let (visibleModal, openAction, closeAction) =
      ModalAction.useModalActionState();

    let onPress = _e => openAction();

    <>
      <View style=styles##button>
        <ButtonAction onPress text=I18n.btn#add_contact icon=Icons.Add.build />
      </View>
      <ModalAction visible=visibleModal onRequestClose=closeAction>
        <ContactFormView action=Create closeAction />
      </ModalAction>
    </>;
  };
};

let styles = Style.(StyleSheet.create({"container": style(~flex=1., ())}));

[@react.component]
let make = () => {
  let aliasesRequest = StoreContext.Aliases.useRequestExceptAccounts();

  <Page>
    <AddContactButton />
    {switch (aliasesRequest) {
     | Done(Ok(aliases), _)
     | Loading(Some(aliases)) =>
       aliases->Map.String.size === 0
         ? <Table.Empty>
             I18n.t#empty_address_book->React.string
           </Table.Empty>
         : aliases
           ->Map.String.valuesToArray
           ->SortArray.stableSortBy((a, b) =>
               Js.String.localeCompare(b.name, a.name)->int_of_float
             )
           ->Array.map(account =>
               <AddressBookRowItem key=(account.address :> string) account />
             )
           ->React.array
     | Done(Error(error), _) =>
       <ErrorView error={error->ErrorHandler.toString} />
     | NotAsked
     | Loading(None) => <LoadingView />
     }}
  </Page>;
};
