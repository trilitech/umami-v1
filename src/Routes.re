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

open ReasonReactRouter;

type t =
  | Accounts
  | Operations
  | AddressBook
  | Delegations
  | Tokens
  | Settings
  | Logs
  | Nft
  | Batch
  | NotFound;

exception RouteToNotFound;

let match = (url: url) => {
  switch (url.hash) {
  | ""
  | "/" => Accounts
  | "/operations" => Operations
  | "/address-book" => AddressBook
  | "/delegations" => Delegations
  | "/tokens" => Tokens
  | "/settings" => Settings
  | "/logs" => Logs
  | "/nft" => Nft
  | "/batch" => Batch
  | _ => NotFound
  };
};

let toHref =
  fun
  | Accounts => "#/"
  | Operations => "#/operations"
  | AddressBook => "#/address-book"
  | Delegations => "#/delegations"
  | Tokens => "#/tokens"
  | Settings => "#/settings"
  | Logs => "#/logs"
  | Nft => "#/nft"
  | Batch => "#/batch"
  | NotFound => raise(RouteToNotFound);

/* This lets us push a Routes.t instead of a string to transition to a new  screen */
let push = route => route |> toHref |> push;

let useHrefAndOnPress = route => {
  let href = toHref(route);
  let onPress = event => {
    event->ReactNative.Event.PressEvent.preventDefault;
    ReasonReactRouter.push(href);
  };
  onPress;
};
