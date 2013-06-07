#import "aslrwrite.h"

%ctor {

NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

NSString *bundleIdentifier = [[NSBundle mainBundle] bundleIdentifier];

if (bundleIdentifier && [bundleIdentifier isEqualToString:@"com.company.app"]) {

writeData(0xADDRESS,0xDATA,BYTESIZE);

}

[pool drain];
}