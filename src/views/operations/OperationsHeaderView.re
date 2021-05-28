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

let styles =
  Style.(
    StyleSheet.create({
      "header":
        style(
          ~marginHorizontal=LayoutConst.pagePaddingHorizontal->dp,
          ~marginTop=LayoutConst.pagePaddingVertical->dp,
          (),
        ),
      "selector":
        style(
          ~alignSelf=`flexStart,
          ~minWidth=390.->dp,
          ~marginTop=0.->dp,
          ~marginBottom=30.->dp,
          (),
        ),
    })
  );

[@react.component]
let make = (~children as right) => {
  <View style=styles##header>
    <Page.Header right>
      <BalanceTotal />
      <AccountSelector style=styles##selector />
    </Page.Header>
    <Table.Head>
      <OperationRowItem.CellType>
        <Typography.Overline3>
          I18n.t#operation_column_type->React.string
        </Typography.Overline3>
      </OperationRowItem.CellType>
      <OperationRowItem.CellAmount>
        <Typography.Overline3>
          I18n.t#operation_column_amount->React.string
        </Typography.Overline3>
      </OperationRowItem.CellAmount>
      <OperationRowItem.CellFee>
        <Typography.Overline3>
          I18n.t#operation_column_fee->React.string
        </Typography.Overline3>
      </OperationRowItem.CellFee>
      <OperationRowItem.CellAddress>
        <Typography.Overline3>
          I18n.t#operation_column_sender->React.string
        </Typography.Overline3>
      </OperationRowItem.CellAddress>
      <OperationRowItem.CellAddress>
        <Typography.Overline3>
          I18n.t#operation_column_recipient->React.string
        </Typography.Overline3>
      </OperationRowItem.CellAddress>
      <OperationRowItem.CellDate>
        <Typography.Overline3>
          I18n.t#operation_column_timestamp->React.string
        </Typography.Overline3>
      </OperationRowItem.CellDate>
      <OperationRowItem.CellStatus>
        <Typography.Overline3>
          I18n.t#operation_column_status->React.string
        </Typography.Overline3>
      </OperationRowItem.CellStatus>
      <OperationRowItem.CellAction />
    </Table.Head>
  </View>;
};
