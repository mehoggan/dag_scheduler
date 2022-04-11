# Change Notes

## Wednesday March 30, 2021
On OSx Big Sur one needs to:
```
sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer
```
after installing xcode from the App Store.

## Wednesday December 1, 2021
* The following file: nboostbeast/include/boost/core/noinit_adaptor.hpp needs
to have its contents changed to

```
template<class U>
void destroy(U* p) {
    using UClass = typename U::U;
    p->~UClass();
    (void)p;
} 
```

in order for it to compile on OSX Big Sur.

## Tuesday August 17, 2021
* Added in ycm changes to support development in Mac OSX.
* Work done on adding tasks to scheduler.
  * Checked interuptible_task_thread for tests.

## WIP: Sunday March 22, 2020
* Added in ycm changes to support development in Ubuntu 18.04 after a long
  break from development. 


# Developer's Notes

## Testing

Please be sure to add /* Tested [✓] */ in the .cxx or .hpp files for methods
that have been tested. Once the class has been completely tested please feel
free to remove the comment.
