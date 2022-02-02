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

module type ICON = {
  let makeProps:
    (
      ~size: float,
      ~color: string=?,
      ~style: ReactNative.Style.t=?,
      ~key: string=?,
      unit
    ) =>
    {
      .
      "size": float,
      "color": option(string),
      "style": option(ReactNative.Style.t),
    };

  let make:
    {
      .
      "size": float,
      "color": option(string),
      "style": option(ReactNative.Style.t),
    } =>
    React.element;
};

module type SVG = {
  let makeProps:
    (
      ~width: ReactNative.Style.size=?,
      ~height: ReactNative.Style.size=?,
      ~fill: string=?,
      ~stroke: string=?,
      ~key: string=?,
      unit
    ) =>
    {
      .
      "fill": option(string),
      "height": option(ReactNative.Style.size),
      "stroke": option(string),
      "width": option(ReactNative.Style.size),
    };

  let make:
    {
      .
      "fill": option(string),
      "height": option(ReactNative.Style.size),
      "stroke": option(string),
      "width": option(ReactNative.Style.size),
    } =>
    React.element;
};

type builder =
  (~color: string=?, ~style: ReactNative.Style.t=?, ~size: float) =>
  React.element;

module Make = (Svg: SVG) => {
  module I = {
    [@react.component]
    let make =
      React.memo(
        (
          ~size: float,
          ~color: option(string)=?,
          ~style: option(ReactNative.Style.t)=?,
        ) => {
        let width = size->Style.dp;
        let height = width;
        let fill = color;
        /* let stroke = color; */
        <View ?style> <Svg width height ?fill /> </View>;
      });
  };

  let build: builder =
    (~color=?, ~style=?, ~size) => {
      let st = style;
      <I style=?st ?color size />;
    };

  include I;
};

module Home = Make(SVGIconHome);
module Send = Make(SVGIconSend);
module History = Make(SVGIconHistory);
module Receive = Make(SVGIconReceive);
module Account = Make(SVGIconAccount);
module AddressBook = Make(SVGIconAddressBook);
module Qr = Make(SVGIconQr);
module Copy = Make(SVGIconCopy);
module Close = Make(SVGIconClose);
module Add = Make(SVGIconAdd);
module ChevronDown = Make(SVGIconChevronDown);
module CheckboxUnselected = Make(SVGIconCheckboxUnselected);
module CheckboxSelected = Make(SVGIconCheckboxSelected);
module Delete = Make(SVGIconDelete);
module Edit = Make(SVGIconEdit);
module ArrowDown = Make(SVGIconArrowDown);
module More = Make(SVGIconMore);
module Delegate = Make(SVGIconDelegate);
module Change = Make(SVGIconChange);
module List = Make(SVGIconList);
module CheckOutline = Make(SVGIconCheckOutline);
module CloseOutline = Make(SVGIconCloseOutline);
module Token = Make(SVGIconToken);
module Tezos = Make(SVGIconTezos);
module ArrowLeft = Make(SVGArrowLeft);
module DarkMode = Make(SVGIconDarkMode);
module OpenExternal = Make(SVGIconOpenExternal);
module Settings = Make(SVGIconSettings);
module RadioOn = Make(SVGIconRadioOn);
module RadioOff = Make(SVGIconRadioOff);
module Refresh = Make(SVGIconRefresh);
module Contract = Make(SVGIconContract);
module Import = Make(SVGIconImport);
module Scan = Make(SVGIconScan);
module Checkmark = Make(SVGIconCheckmark);
module Show = Make(SVGIconShow);
module Hide = Make(SVGIconHide);
module Stop = Make(SVGIconStop);
module AddContact = Make(SVGIconAddContact);
module Logs = Make(SVGIconLogs);
module Previous = Make(SVGIconPrevious);
module Next = Make(SVGIconNext);
module FirstPage = Make(SVGIconFirstPage);
module LastPage = Make(SVGIconLastPage);
module Ledger = Make(SVGIconLedger);
module QuestionMark = Make(SVGIconQuestionMark);
module AddToken = Make(SVGIconTokenAdd);
module Options = Make(SVGIconOption);
module EyeStrike = Make(SVGIconEyeStrike);
module Eye = Make(SVGIconEye);
module Search = Make(SVGIconSearch);
module Nft = Make(SVGIconNft);
module NoImg = Make(SVGIconNoImg);
module MagnifierPlus = Make(SVGIconMagnifierPlus);
module Gallery = Make(SVGIconGallery);
module Collection = Make(SVGIconCollection);
module Info = Make(SVGIconInfo);
module SyncNFT = Make(SVGIconSyncNft);
module Ok = Make(SVGIconOk);
module Code = Make(SVGIconCode);
module Metadata = Make(SVGIconMetadata);
module SwitchOn = Make(SVGSwitchOn);
module SwitchOff = Make(SVGSwitchOff);
module ArrowUp = Make(SVGIconArrowUp);
module Cloud = Make(SVGIconCloud);
