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

// internally, Map.Int is better than Set if we want to merge it with a
// Registered Map. See TokenRegistry.Registered.updateNFTsVisility
type t = PublicKeyHash.Map.map(Map.Int.t(unit));

let size = selected =>
  selected->PublicKeyHash.Map.reduce(0, (size, _, c) =>
    size + c->Map.Int.size
  );

let isSelected = (selected, pkh, tokenId) =>
  selected
  ->PublicKeyHash.Map.get(pkh)
  ->Option.map(ids => ids->Map.Int.has(tokenId))
  != None;

let updateSelection = (selected, pkh, id, checked) =>
  selected->PublicKeyHash.Map.update(
    pkh,
    fun
    | None => checked ? Some(Map.Int.empty->Map.Int.set(id, ())) : None
    | Some(ids) =>
      checked
        ? Some(ids->Map.Int.set(id, ())) : Some(ids->Map.Int.remove(id)),
  );

let singleton = (pkh, id) =>
  PublicKeyHash.Map.empty->PublicKeyHash.Map.set(
    pkh,
    Map.Int.empty->Map.Int.set(id, ()),
  );

let isEmpty = selected =>
  selected->PublicKeyHash.Map.reduce(true, (isEmpty, _, ids) =>
    isEmpty && ids->Map.Int.isEmpty
  );
