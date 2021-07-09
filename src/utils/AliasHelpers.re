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

let getAliasFromAddress = (address: PublicKeyHash.t, aliases) => {
  aliases
  ->Map.String.get((address :> string))
  ->Option.map((account: Alias.t) => account.name);
};

let getContractAliasFromAddress = (address: PublicKeyHash.t, aliases, tokens) => {
  let r =
    aliases
    ->Map.String.get((address :> string))
    ->Option.map((account: Alias.t) => account.name);

  switch (r) {
  | None =>
    tokens
    ->Map.String.get((address :> string))
    ->Option.map((token: Token.t) => I18n.t#token_contract(token.alias))
  | Some(r) => Some(r)
  };
};

let formCheckExists = (aliases, alias): ReSchema.fieldState => {
  aliases->Map.String.some((_, v: Alias.t) => v.name == alias)
    ? Error(I18n.form_input_error#name_already_registered) : Valid;
};
