# Security Policy

This is not a serious kernel, and hence we do not have any serious security
policy. There are of course many things that could compromise a toy kernel such
as this. For example:

- Bad bound checks that could turn into a vector of attacks.
- Bad enforcements of memory protection.
  - Remember that this kernel does not implement any memory protection on the
    purpose of being simple, but we could always enable other mechanisms to keep
    things "secure".
- Other kind of bugs that could compromise such as kernel.

For any of these situations, since this is a toy project, simply submit an
[issue to the Github repository](https://github.com/mssola/fbos/issues).
