# TermWriter Demo
A showcase of the buffered markdown-to-terminal engine.
Write content like a .md file, parseFile() renders it.
---

## Headings
### This is an H3
Regular text below a heading.

## Blockquotes
> A blockquote is just a line starting with >
> Multiple lines are separate quotes.

## Callout Blocks

>![danger] Shallow Copy Warning
> Shallow copy: two Dogs share one Brain.
> Delete one => dangling pointer in the other.
> Delete both => double free => crash.
>

>![tip] Pro Tip
> Use >![type] to open a callout.
> Lines starting with > become the body.
> An empty > closes the block.
>

>![note] Did you know?
> You can register as many callout types as you want.
> Each one has its own colors, glyph, and background.
>

## Log Levels
!i This is an info message
!! This is a warning
!x This is an error
!v This is a success

## Unordered List
- Virtual destructors prevent memory leaks
- Deep copy allocates new resources
- Abstract classes enforce contracts
- Interfaces decouple API from implementation

## Ordered List
1. Default constructor
2. Copy constructor (deep copy)
3. Copy assignment operator (deep copy)
4. Destructor (release resource)

## Sections
$$ Virtual (correct) / delete dogPtr => ~Dog() then ~Animal()

$$ Non-virtual (WRONG) / delete wrongCatPtr => ~WrongAnimal() ONLY

## Inline Styles
**This line is bold**
*This line is italic*
~~This line is strikethrough~~
~This line is dim~

## Separators
---
A separator above, a heavy rule below.
===

# Final Thoughts
> The goal is not just to make it compile.
> It is to understand WHY each mechanism exists.
===
~Built with TermWriter + TermStyle  |  C++98  |  42 school~
